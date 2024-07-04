import java.io.IOException;
import java.util.HashMap;
import java.util.Stack;

public class Interpreter {
    private static final char OPERATOR_RIGHT = '>';
    private static final char OPERATOR_LEFT = '<';
    private static final char OPERATOR_INCREMENT = '+';
    private static final char OPERATOR_DECREMENT = '-';
    private static final char OPERATOR_WRITE = '.';
    private static final char OPERATOR_READ = ',';
    private static final char OPERATOR_JUMP_IF_ZERO = '[';
    private static final char OPERATOR_JUMP_UNLESS_ZERO = ']';

    private static final int CELL_MIN_VALUE = 0;
    private static final int CELL_MAX_VALUE = 255;

    private String source;
    private int cells[];
    // You can also initialize instance variables inline. Similar to TS/JS,
    // itialization code will be run every time the class is instantiated.
    private int pc = 0;
    private int pointer = 0;
    private HashMap<Integer, Integer> bracketMap = new HashMap<>();

    public Interpreter(String source) {
        this.source = source;
        // Array default values use the default value of their element type.
        // ints (numeric types in general) default to 0, so there's no need to
        // explicitly zero out the array. Also note that we use the int type
        // despite there being a byte type because all numeric types are signed
        // in Java, so byte would range from [-128, 127] instead of [0, 255].
        this.cells = new int[4096];
        initBracketMap();
    }

    // There is no support for default arguments, so we have to use overloading.
    public Interpreter(String source, int memorySize) {
        this.source = source;
        this.cells = new int[memorySize];
        initBracketMap();
    }

    public String getSource() {
        return this.source;
    }

    public void run() throws IOException {
        while (this.pc < this.source.length()) {
            char operator = this.source.charAt(this.pc);

            switch (operator) {
                case OPERATOR_RIGHT:
                    movePointerRight();
                    break;
                case OPERATOR_LEFT:
                    movePointerLeft();
                    break;
                case OPERATOR_INCREMENT:
                    incrementCell();
                    break;
                case OPERATOR_DECREMENT:
                    decrementCell();
                    break;
                case OPERATOR_WRITE:
                    writeCellToStdout();
                    break;
                case OPERATOR_READ:
                    replaceCellWithStdin();
                    break;
                case OPERATOR_JUMP_IF_ZERO:
                    jumpIfZero();
                    break;
                case OPERATOR_JUMP_UNLESS_ZERO:
                    jumpIfNonzero();
                    break;
            }

            this.pc++;
        }
    }

    private void initBracketMap() {
        // The type parameter list seems to be optional/inferred on the RHS.
        Stack<Integer> seenOpeners = new Stack<>();

        for (int address = 0; address < this.source.length(); address++) {
            char operator = this.source.charAt(address);

            if (operator == OPERATOR_JUMP_IF_ZERO) {
                seenOpeners.push(address);
                continue;
            }

            if (operator == OPERATOR_JUMP_UNLESS_ZERO) {
                if (seenOpeners.isEmpty()) {
                    System.err.println("Brainfuck syntax error: mismatched "
                            + OPERATOR_JUMP_UNLESS_ZERO);
                    System.exit(1);
                }
                int openerAddress = seenOpeners.pop();

                this.bracketMap.put(openerAddress, address);
                this.bracketMap.put(address, openerAddress);
            }
        }

        if (!seenOpeners.isEmpty()) {
            System.err.println("Brainfuck syntax error: mismatched "
                    + OPERATOR_JUMP_IF_ZERO);
            System.exit(1);
        }
    }

    private void movePointerRight() {
        this.pointer = (this.pointer + 1) % this.cells.length;
    }

    private void movePointerLeft() {
        if (this.pointer == 0) {
            this.pointer = this.cells.length - 1;
        } else {
            this.pointer--;
        }
    }

    private void incrementCell() {
        int cell = this.cells[this.pointer];
        this.cells[this.pointer] = (cell == CELL_MAX_VALUE)
                ? CELL_MIN_VALUE
                : cell + 1;
    }

    private void decrementCell() {
        int cell = this.cells[this.pointer];
        this.cells[this.pointer] = (cell == CELL_MIN_VALUE)
                ? CELL_MAX_VALUE
                : cell - 1;
    }

    private void writeCellToStdout() {
        int cell = this.cells[this.pointer];
        System.out.print((char) cell);
    }

    private void replaceCellWithStdin() throws IOException {
        int data = System.in.read();
        if (data == -1) {
            throw new IOException("EOF");
        }
        this.cells[this.pointer] = data;
    }

    private void jumpIfZero() {
        if (this.cells[this.pointer] == 0) {
            int destinationAddress = this.bracketMap.get(this.pc);
            this.pc = destinationAddress;
        }
    }

    private void jumpIfNonzero() {
        if (this.cells[this.pointer] != 0) {
            int destinationAddress = this.bracketMap.get(this.pc);
            this.pc = destinationAddress;
        }
    }
}
