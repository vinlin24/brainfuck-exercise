#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Unit tester for Brainfuck interpreter implementations."""

import subprocess
import unittest
from argparse import ArgumentParser
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, Type

TESTER_PATH = Path(__file__)
TESTS_DIRECTORY = TESTER_PATH.parent
PROJECT_DIRECTORY = TESTS_DIRECTORY.parent


@dataclass
class TestCaseConfig:
    test_name: str
    source_path: str
    expected_stdout: str
    expected_exit: int


def get_test_cases() -> list[TestCaseConfig]:
    test_cases = list[TestCaseConfig]()

    source_paths = set[Path]()
    stdout_paths = set[Path]()
    error_code_paths = set[Path]()

    for element_path in TESTS_DIRECTORY.iterdir():
        if element_path.suffix == ".bf":
            source_paths.add(element_path)
        elif element_path.suffix == ".out":
            stdout_paths.add(element_path)
        elif element_path.suffix == ".err":
            error_code_paths.add(element_path)

    for source_path in source_paths:
        stdout_path = source_path.with_suffix(".out")
        if stdout_path in stdout_paths:
            expected_stdout = stdout_path.read_text("utf-8")
        else:
            expected_stdout = ""

        error_code_path = source_path.with_suffix(".err")
        if error_code_path in error_code_paths:
            expected_exit = int(error_code_path.read_text("utf-8"))
        else:
            expected_exit = 0

        test_case = TestCaseConfig(
            test_name=source_path.stem,
            source_path=str(source_path),
            expected_stdout=expected_stdout,
            expected_exit=expected_exit,
        )
        test_cases.append(test_case)

    return test_cases


def create_interpreter_test_case(
    language_directory: Path,
    test_case_config: TestCaseConfig,
) -> Type[unittest.TestCase]:
    class TestInterpreterTestCase(unittest.TestCase):
        def _run_interpreter(self) -> subprocess.CompletedProcess[bytes]:
            run_script = language_directory / "run.sh"
            return subprocess.run(
                # NOTE: I would've liked to be able to run the script
                # directly and let the shebang specify the interpreter,
                # but that causes WinError 193 on Windows. Thus, we
                # hard-code `bash` an the run script interpreter.
                ["bash", str(run_script), test_case_config.source_path],
                check=False,
                capture_output=True,
            )

        def test_case(self) -> None:
            process = self._run_interpreter()

            received_stdout = process.stdout.decode("utf-8")
            expected_stdout = test_case_config.expected_stdout

            received_exit = process.returncode
            expected_exit = test_case_config.expected_exit

            self.assertEqual(received_exit, expected_exit)
            self.assertEqual(received_stdout, expected_stdout)

    return TestInterpreterTestCase


def create_all_test_cases_for_language(
    language_name: str,
) -> list[Type[unittest.TestCase]]:
    test_case_configs = get_test_cases()

    test_case_classes = list[Type[unittest.TestCase]]()

    for test_case_config in test_case_configs:
        language_directory = PROJECT_DIRECTORY / language_name
        test_case_class = create_interpreter_test_case(
            language_directory,
            test_case_config,
        )

        test_case_name = test_case_config.test_name
        # Set a readable test case name for when verbose is enabled.
        test_case_class.__qualname__ = f"{language_name}_{test_case_name}"
        test_case_classes.append(test_case_class)

    return test_case_classes


def run_unit_tests(
    test_suite_classes: Iterable[Type[unittest.TestCase]],
    verbose: bool,
) -> None:
    """Start the unittest runtime.

    Note that we cannot just use `unittest.main()` since that parses
    command line arguments, interfering with our argparse CLI.
    Furthermore, we have dynamic `TestCase`s that cannot be discovered
    anyway as they need to be created through factory functions.
    """
    loader = unittest.TestLoader()
    test_suites = [
        loader.loadTestsFromTestCase(cls)
        for cls in test_suite_classes
    ]
    all_tests_suite = unittest.TestSuite(test_suites)

    test_runner = unittest.TextTestRunner(verbosity=(2 if verbose else 1))
    test_runner.run(all_tests_suite)


def discover_implementation_directories() -> list[str]:
    directory_names = list[str]()
    for element_path in PROJECT_DIRECTORY.iterdir():
        if not element_path.is_dir() or element_path.name in {"tests", ".git"}:
            continue
        directory_names.append(element_path.name)
    return directory_names


def init_parser(implementation_directories: list[str]) -> ArgumentParser:
    parser = ArgumentParser(description=__doc__)
    parser.add_argument(
        "language_directories",
        metavar="LANG",
        # NOTE: This still seems to act like nargs="+". See:
        # https://utcc.utoronto.ca/~cks/space/blog/python/ArgparseNargsChoicesLimitation.
        # I would've liked to make it so that providing no arguments
        # means "run all", but due to this bug/limitation, we make "all"
        # an option and use default= as a workaround.
        nargs="*",
        default="all",
        choices=["all"] + implementation_directories,
    )
    parser.add_argument(
        "-v",
        "--verbose",
        action="store_true",
    )
    return parser


def main() -> None:
    implementation_directories = discover_implementation_directories()
    parser = init_parser(implementation_directories)
    args = parser.parse_args()

    language_directories: list[str] = args.language_directories
    verbose: bool = args.verbose

    if "all" in language_directories:
        language_directories = implementation_directories

    test_case_classes_to_use = list[Type[unittest.TestCase]]()
    for directory_name in language_directories:
        test_case_classes = create_all_test_cases_for_language(directory_name)
        test_case_classes_to_use.extend(test_case_classes)

    run_unit_tests(test_case_classes_to_use, verbose)


if __name__ == "__main__":
    main()
