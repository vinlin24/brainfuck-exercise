using System;
using System.IO;

class Brainfuck
{
    public static void Main(string[] args)
    {
        if (args.Length < 1)
        {
            PrintUsage();
            Environment.Exit(1);
        }
        string sourcePath = args[0];
        string source = File.ReadAllText(sourcePath);
        Interpreter interpreter = new(source);
        interpreter.Run();
    }

    private static void PrintUsage()
    {
        Console.WriteLine("usage: dotnet run FILE");
    }
}
