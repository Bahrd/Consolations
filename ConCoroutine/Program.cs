using System;
using System.Linq;
using System.Diagnostics;
/// Coroutines - generators and servers...
/// https://docs.microsoft.com/en-us/dotnet/csharp/language-reference/keywords/yield#examples
namespace ConCoroutine
{
    using SCGI = System.Collections.Generic.IEnumerable<int>;
    class Program
    {
        static void Main()
        {
            var (AllAboutThatBase, TheExpo) = (0x2, 0x8);                   
            
            // Coroutines...
            foreach (var n in LazyPower(AllAboutThatBase, TheExpo))
                Console.Write("{0:x}\t", n);
            Console.WriteLine();
            
            // Coroutines and PLINQ... 
            foreach (var n in NotSoLazyGenerator(Stop: TheExpo)
                              .AsParallel()
                              .WithDegreeOfParallelism(0x8)
                              .WithExecutionMode(ParallelExecutionMode.ForceParallelism)
                              .OrderByDescending(n => n)
                              .Select(n => AgilePower(AllAboutThatBase, n)))
                Console.Write("{0:x}\t", n);
            Console.WriteLine();
            
            // In old-C style...
            for (var n = 0x0; n < TheExpo; Console.Write("{0:x}\t", 0x1 << n), ++n);
            Console.WriteLine();
            
            // Coroutines and LINQ... Take I
            foreach (var n in from m in NotSoLazyGenerator(TheExpo)
                              orderby m ascending
                              select AgilePower(AllAboutThatBase, m))
                Console.Write("{0:x}\t", n);
            Console.WriteLine();
            
            // Coroutines and LINQ... Take II
            foreach (var n in NotSoLazyGenerator(Stop: TheExpo)
                              .OrderByDescending(n => n)
                              .Select(n => AgilePower(AllAboutThatBase, n)))
                Console.Write("{0:x}\t", n);
            Console.WriteLine();
            
            // Neither coroutines nor (P)LINQ...
            for (var n = 0x0; n < TheExpo; ++n)
                Console.Write("{0:x}\t", AgilePower(AllAboutThatBase, n));
            Console.WriteLine();
        }
        public static SCGI NotSoLazyGenerator(int Stop, int Start = 0x0, int Step = 0x1)
        {
            // Are we sure '!=' is a bullet-proof stopping condition?
            Debug.Assert(Math.Sign(Stop - Start) == Math.Sign(Step), "Ain't gonna end...");
            Debug.Assert(unchecked(((Stop - Start)/(double)Step) == (Stop - Start)/Step), "Ain't really gonna end...");
            for (var n = Start; n != Stop; n += Step)
                yield return n;
        }
        public static SCGI LazyPower(int Base, int Expo)
        {
            for (var (n, result) = (0x0, 0x1); n < Expo; result *= Base, n++)
                yield return result;
        }
        public static int AgilePower(int Base, int Expo)
        {
            var (n, result) = (0x0, 0x1); 
            for (; n < Expo; result *= Base, n++);
                return result;
        }
    }
    // https://learn.microsoft.com/en-us/dotnet/csharp/language-reference/statements/yield
}
/**
 * cd .\Consolations\ConCoroutine\
 * dotnet run --no-build
 */