using System;
using System.Linq;

/// Coroutines - generators and servers...
/// https://docs.microsoft.com/en-us/dotnet/csharp/language-reference/keywords/yield#examples
namespace ConCoroutine
{
    class Program
    {
        static void Main()
        {
            var (AllAboutThatBase, TheExpo) = (0x2, 0x8);            
            foreach (var n in LazyPower(AllAboutThatBase, TheExpo))  //.Where(x => (x < 0x40)).Select(x => Math.Abs(x)))
                Console.Write("{0:x} ", n);
            Console.WriteLine();
            for (var n = 0x0; n < TheExpo; ++n)
                Console.Write("{0:x} ", AgilePower(AllAboutThatBase, n));
            Console.WriteLine();
        }
        public static System.Collections.Generic.IEnumerable<int> LazyPower(int Base, int Expo)
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
}
