
using System.Collections.Generic;

/** ArithmetiC
 * * 
 * * A (much too) simple arithmetic coding algorithm illustration
 * 
 *   Be careful! The original code† has heavily been modified with the help of Copilot (AD 2024).
 *   In particular, a few comment suggestions were taken literally. 
 *   ------------------------------------------------------------
 *   † Created in that (relatively) short and (essentially) eventless time period 
 *     between the Chicxulub impact 65mya and 2010 AC.
 */
namespace ArithmeticCoding
{
    class AritmeticCoder : IDisposable
    {
        #region Fields of AC
        string text;
        decimal code, C, A;
        readonly List<decimal> fx = [], Fx = [0m];

        /* ‡ The original example had even 26 letters, but it was too many for a demo. 
         *   Unfortunately, the a,b and c have noting to do with the famous 'abc conjecture'...
         *   https://en.wikipedia.org/wiki/Abc_conjecture
         *   abc should not be confused with the BAC or CABAC coders either
         *   https://en.wikipedia.org/wiki/Context-adaptive_binary_arithmetic_coding
         *
         *   Here we use a three letter alphabet: a, b, c‡.
         *   If the probabilities satisfy the inequality P(a) >> P(b) + P(c) 
         *   (so that the entropy of the source is close(r) to zero),
         *   the advantage of arithmetic coding over any block coding algorithm 
         *   can be seen in its full glory.
         *   
         *   https://en.wikipedia.org/wiki/Feigenbaum_constants#Value - the source of the "magic" constant
             const int size = 3; 
             const double p = 1 / 4.669201609102990671853203820466;
             const double pa = 1.0 - p, pb = 3 * p / 4, pc = p / 4;
         */
        // BAC
        const int size = 2;
        readonly double p;
        readonly double pa, pb;
        // Administrative stuff
        readonly int[] freqMap = new int[size];
        readonly List<int> freqUnmap = [0, 1];
        readonly List<double> freqs;

        #region EnglishAlphabetFrequencies
        /**
         * "Etaoin Shrdlu"
         * Taken from http://rinkworks.com/words/letterfreq.shtml
         * See also: https://en.wikipedia.org/wiki/Letter_frequency#Relative_frequencies_of_letters_in_other_languages
         */
        /*      const int size = 26;

              int[] freqMap = new int[size];
              int[] freqUnmap = new int[size]    { 
                                                        4	,
                                                        0	,
                                                        8	,
                                                        17	,
                                                        19	,
                                                        14	,
                                                        13	,
                                                        18	,
                                                        11	,
                                                        2	,
                                                        20	,
                                                        15	,
                                                        12	,
                                                        3	,
                                                        7	,
                                                        6	,
                                                        1	,
                                                        24	,
                                                        5	,
                                                        21	,
                                                        22	,
                                                        10	,
                                                        23	,
                                                        25	,
                                                        16	,
                                                        9	
                                                       };


              double[] freqs = new double[size] 
                                                      {
                                                        0.114212990689824	,
                                                        0.085574671966026	,
                                                        0.0794495562693962	,
                                                        0.0751483639135406	,
                                                        0.0745834921326292	,
                                                        0.0712282898676975	,
                                                        0.0640619045026406	,
                                                        0.0554935482114662	,
                                                        0.0552213208471716	,
                                                        0.0474220068601296	,
                                                        0.0365669407088801	,
                                                        0.0327285348723254	,
                                                        0.0322249142483802	,
                                                        0.0312789241574563	,
                                                        0.0276174661076931	,
                                                        0.0230032122828987	,
                                                        0.0212405400990908	,
                                                        0.0199950999074427	,
                                                        0.0146798606195895	,
                                                        0.0107121467849948	,
                                                        0.00944628954102466	,
                                                        0.00840501987259759	,
                                                        0.0034504818424348	,
                                                        0.00242282354222246	,
                                                        0.0023343496488267	,
                                                        0.00149725050362062	
                                                      };
        */
        #endregion
        #endregion
        public AritmeticCoder(double prob, string txt)
        {
            // Administrative stuff
            p = prob; 
            (pa, pb) = (1.0 - p, p);
            freqs = [pa, pb];

            // An initialization of the class fields (it's a constructor, after all!)    
            (text, code, (C, A)) = (txt, 0.0m, (0.0m, 1.0m));

            //  And a couple of for-loops types we all know and love (sometimes too much)...
            //  1. An old school (one liner - like in ol'good days)...
            for (var index = 0; index < size; freqMap[freqUnmap[index]] = index, ++index) ;

            /*  2. ... vs. LINQ style (ThnX, Copilot, for a tip!)
                BTW, try at home (and don't take it outside!) a version
                without quantized probabilities... Good luck!
                See also:
                https://stackoverflow.com/questions/46498743/what-is-the-stdpartial-sum-equivalent-in-c

                A helper lambda function for quantizing probabilities (okay in C# 12)
                https://learn.microsoft.com/en-us/dotnet/csharp/whats-new/csharp-12#default-lambda-parameters
                (with a dozen of [significant rather than dirty] bits as a default value)
            */
            var Qx = (decimal _x, int _p = 12) => { var p = 1 << _p; return Math.Floor(_x * p)/p; };
            var _fx = from fq in freqs
                      let _ = Convert.ToDecimal(fq)
                      select Qx(_);
            var _Fx = _fx.Select((_, index) => _fx.Take(index).Sum()); // ... and their partial sums
            (fx, Fx) = (_fx.ToList(), _Fx.ToList());

            // 3. ... and a for-each                 Fx[^1]
            //foreach (var f in fx[0..^1]) Fx.Add(Fx.Last() + f); // Cumulated quantized frequencies
        }
        ~AritmeticCoder()
        {
            Dispose();
        }
        public void Dispose()
        {
            Console.WriteLine("♪♫ Che sarà, sarà! ♫♪ ");
            GC.SuppressFinalize(this);
        }
        public void Encode()
        {
            // A fun from a few functions...
            Action<(decimal, decimal), string> ThrowException = (_CA, _txt) =>
            {
                var (C, A) = _CA;
                var msg = $"\nMessage '{_txt}' contains too much information for a given arithmetic p: "
                        + $"the resulting coding interval ({C}, {C + A}) is not wide enough...";
                throw new Exception(msg);
            };
            static void PrintInterval(decimal C, decimal A, int width = 0x100 - 0b100)
            {                                        // ('1 +' here and there is just for aestetics)
                var (c, a) = (Convert.ToInt32(C * width), 1 + Convert.ToInt32(A * width));
                Console.WriteLine("[" + new string(' ', c) + new string('.', a)
                                      + new string(' ', 1 + width - c - a)
                                      + $") [{C}, {C + A})");
            }

            // End of fun... go work!
            var width = 0x100 - 0b100;
            // A bit of ASCII art
            Console.WriteLine("0" + new string('.', width + 1) + "1");

            foreach (char letter in text)
            {
                var index = freqMap[letter - '0'];

                C += Fx[index] * A;
                A *= fx[index];

                // Exception handling (testing whether the interval is wide enough)
                if (A / 2m == 0m) ThrowException((C, A), text);

                // Visual representation of the code interval
                PrintInterval(C, A);
            }
            // The ultimate code can be any number from the middle of the interval
            // (provided that the interval still has a middle! ;)
            code = C + (A / 2m);
            return;        
        }
        public void Decode()
        {
            // A couple of helper functions
            int Extract(decimal val) => Array.FindLastIndex(Fx.ToArray(), x => x < val);
            char Letter(int index) => Convert.ToChar(freqUnmap[index] + '0');

            var lenght = text.Length;
            text = "";
            while(lenght-- != 0)
            {
                var index = Extract(code);

                code -= Fx[index];
                code /= fx[index];

                text += Letter(index);
            }
        }
        public void EncodingReport()
        {
            Console.WriteLine($"\n\nIn the 7-bit ASCII code this message has {text.Length * 7} bits");
            int bits_size = Convert.ToInt32(Math.Ceiling(Math.Log(size, 2)));
            Console.WriteLine($"\n\nIn the fixed-size {bits_size}-bit code this message has {text.Length * bits_size} bits");
            Console.WriteLine($"The arithmetic code of \n\n{text}\n\nis any number from interval\n[{C}, {C + A})");
            Console.WriteLine($"\n\nFor instance\n{code}\n");
            int messageLength = Convert.ToInt32(Math.Ceiling(-Math.Log(Convert.ToDouble(A), 2)));
            Console.WriteLine($"Which binary representation needs 'only' (wait for it...) {messageLength} bits\n");
        }
        public void DecodingReport()
        {
            Console.WriteLine($"\n\nThe decoded message is:\n{text}\n");
        }
        static void Main(string []args)
        {
            try
            {
                while (true)
                {
                    // For those for whom ASCII is not enough
                    Console.OutputEncoding = System.Text.Encoding.UTF8;
                    // Bureaucracy and bookkeeping
                    var (pr, cooltura) = args.Length > 1 ? (args[0], args[1])
                                                         : ("1.0/2.0", "en-US");   
                    // https://stackoverflow.com/questions/13354211/how-to-set-default-culture-info-for-entire-c-sharp-application
                    var coolUS = System.Globalization.CultureInfo.CreateSpecificCulture(cooltura);
                    Thread.CurrentThread.CurrentCulture = coolUS;

                    // Now we are convertin'!
                    var p = Convert.ToDouble(pr);
                    // 'Nullity' check first... Termination check second...
                    // 'EOC' stands for 'End Of Coding'.
                    if (Console.ReadLine() is string text && text != "" && !text.EndsWith("EOC"))
                    {
                        var Hp = -p * Math.Log(p, 2) - (1 - p) * Math.Log(1 - p, 2);
                        Console.WriteLine($"P(1) = {p:0.###}, H(p) = {Hp:0.###} [bit/s]");

                        // ... and the real stuff
                        using AritmeticCoder codec = new(p, text);
                        codec.Encode();
                        codec.EncodingReport();

                        codec.Decode();
                        codec.DecodingReport();
                    }
                    else break;
                }
            }
            catch (Exception e)
            {
                Console.Error.WriteLine($"\nExpected exception: {e.Message}\n");
            }
        }
    }
}