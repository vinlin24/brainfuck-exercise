#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Integration tester for Brainfuck interpreter implementations."""

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
    source_paths = [
        path for path in TESTS_DIRECTORY.iterdir()
        if path.suffix == ".bf"
    ]
    test_cases = list[TestCaseConfig]()

    for source_path in source_paths:
        stdout_path = source_path.with_suffix(".out")
        if stdout_path.is_file():
            expected_stdout = stdout_path.read_text("utf-8")
        else:
            expected_stdout = ""

        error_code_path = source_path.with_suffix(".err")
        if error_code_path.is_file():
            expected_exit = int(error_code_path.read_text("utf-8"))
        else:
            expected_exit = 0

        test_cases.append(TestCaseConfig(
            test_name=source_path.stem,
            source_path=str(source_path),
            expected_stdout=expected_stdout,
            expected_exit=expected_exit,
        ))

    return test_cases


def execute_shell_script(
    script_path: str | Path,
    *arguments: str,
) -> subprocess.CompletedProcess[bytes]:
    return subprocess.run(
        # NOTE: I would've liked to be able to run the script directly
        # and let the shebang specify the interpreter, but that causes
        # WinError 193 on Windows. Thus, we hard-code `bash` as the
        # interpreter for all shell scripts.
        ["bash", script_path, *arguments],
        check=False,
        capture_output=True,
    )


def create_interpreter_test_case(
    language_directory: Path,
    test_case_config: TestCaseConfig,
    setup_error: str | None = None,
) -> Type[unittest.TestCase]:
    @unittest.skipIf(
        setup_error is not None,
        reason=f"Setup script errored: {setup_error}",
    )
    class TestInterpreterTestCase(unittest.TestCase):
        def _run_interpreter(self) -> subprocess.CompletedProcess[bytes]:
            run_script = language_directory / "run.sh"
            brainfuck_source_path = test_case_config.source_path
            return execute_shell_script(run_script, brainfuck_source_path)

        def test_case(self) -> None:
            process = self._run_interpreter()

            received_stdout = process.stdout.decode("utf-8")
            expected_stdout = test_case_config.expected_stdout

            received_exit = process.returncode
            expected_exit = test_case_config.expected_exit

            self.assertEqual(received_exit, expected_exit)
            self.assertEqual(received_stdout, expected_stdout)

    return TestInterpreterTestCase


def run_language_setup_if_exists(
    language_directory: Path,
    verbose: bool,
) -> str | None:
    """
    Run the language directory's set up script, if exists. Return `None`
    if it succeeds or the script does not exist (skipped). Otherwise,
    return the stderr of the subprocess.
    """
    setup_script = language_directory / "setup.sh"

    if not setup_script.is_file():
        return None

    if verbose:
        print(f"Running {setup_script} ... ", end="")
    process = execute_shell_script(setup_script)
    if verbose:
        print("Done.")

    if process.returncode != 0:
        return process.stderr.decode("utf-8")
    return None


def create_all_test_cases_for_language(
    language_name: str,
    verbose: bool,
) -> list[Type[unittest.TestCase]]:
    language_directory = PROJECT_DIRECTORY / language_name
    setup_error = run_language_setup_if_exists(language_directory, verbose)

    test_case_configs = get_test_cases()
    test_case_classes = list[Type[unittest.TestCase]]()

    for test_case_config in test_case_configs:
        test_case_class = create_interpreter_test_case(
            language_directory,
            test_case_config,
            setup_error,
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
        test_case_classes = create_all_test_cases_for_language(
            directory_name,
            verbose,
        )
        test_case_classes_to_use.extend(test_case_classes)

    run_unit_tests(test_case_classes_to_use, verbose)


if __name__ == "__main__":
    main()
