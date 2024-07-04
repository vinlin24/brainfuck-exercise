// Note that we do not need to import classes from other project files as long
// as they're compiled as part of the same package.

// Import statements seem to bring the last part of the name into the current
// namespace e.g. `IOException`.
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

public class Brainfuck {
    public static void main(String[] args) {
        if (args.length < 1) {
            System.err.println(getUsage());
            System.exit(1);
        }

        Path sourcePath = Paths.get(args[0]);
        // Remember that since String is technically an Object type, it can hold
        // the null value.
        String source = null;
        try {
            source = slurpFile(sourcePath);
        } catch (IOException error) {
            error.printStackTrace();
            System.exit(1);
        }

        Interpreter interpreter = new Interpreter(source);
        try {
            interpreter.run();
        } catch (IOException error) {
            error.printStackTrace();
            System.exit(1);
        }
    }

    private static String getUsage() {
        // There doesn't seem to be a straightforward analog of argv[0] in Java.
        return "usage: path/to/program FILE";
    }

    // In Java, all methods that can throw CHECKED exceptions must be marked
    // with `throws`. This enforces that non-`throws` methods truly never throw
    // such exceptions because they have to explicitly catch all parts that can
    // `throw` or there will be a compiler error.
    private static String slurpFile(Path filePath) throws IOException {
        return Files.readString(filePath);
    }

    // As continuation, the types of exceptions are:
    //
    // 1. CHECKED. They extend `Exception`. Example: `IOException`. They
    // represent recoverable conditions. They are checked at compile time and
    // require handling.
    // 2. UNCHECKED. They extend `RuntimeException`. Example:
    // `NullPointerException`. They represent programming errors, like logic
    // errors or improper use of an API. They do not require handling (better
    // practice to change the code to avoid them in the first place).
    // 3. ERRORS. They extend `Error`. Example: `OutOfMemoryError`. They
    // represent serious system issues related to the runtime environment, out of
    // the application's control. They generally do not require handling.
}
