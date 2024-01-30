/** ArithmetiC
 * * 
 * * A (much too) simple arithmetic coding algorithm illustration
 * 
 *   Be careful! The original code† has heavily been modified with the help of Copilot (AD 2024).
 *   In particular, the suggested comments were sometimes taken literally. 
 *   ------------------------------------------------------------
 *   † Created sometime in that short and eventless period between the Chicxulub impact and 2010 AC.
 */

namespace ArithmeticCoding
{
    class AritmeticCoder : IDisposable
    {
        string text;
        decimal code;
        decimal C, A;
        readonly List<decimal> fx = new(), Fx = new() { 0m };

        /*   Here we use a three letter alphabet: a, b, c‡.
         *   If the probabilities satisfy the inequality P(a) > P(b) + P(c) 
         *   (so that the entropy of the source is close(r) to zero),
         *   the advantage of arithmetic coding over any block coding algorithm 
         *   can be seen in its full glory.
         */
        const int size = 3; const double p = 1 / 4.0;
        const double pa = 1.0 - p, pb = 3 * p / 4, pc = p / 4;
        /* ‡ The original example had 26 letters, but it was too many for a demo. 
         *   Unfortunately, the a,b and c have noting to do with the famous 'abc conjecture'...
         *   https://en.wikipedia.org/wiki/Abc_conjecture
         *   abc should not be confused with the BAC or CABAC coders either
         *   https://en.wikipedia.org/wiki/Context-adaptive_binary_arithmetic_coding
         */
        // Administrative stuff
        readonly int[] freqMap = new int[size],
                       freqUnmap = new int[size] { 0, 1, 2 };
        readonly double[] freq = new double[size] { pa, pb, pc };
        #region EnglishAlphabetFrequencies
        /**
         * "Etaoin Shrdlu"
         * Taken from http://rinkworks.com/words/letterfreq.shtml
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


              double[] freq = new double[size] 
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
        public AritmeticCoder(string args)
        {
            (text, code, (C, A)) = (args, 0.0m, (0.0m, 1.0m));

            //  A couple of for-loops types we all know and love (sometimes too much)...
            //  An old school (one liner - like in ol'good days)...
            for (var index = 0; index < size; freqMap[freqUnmap[index]] = index, ++index) ;

            //  ... vs. LINQ style (ThnX, Copilot, for a tip!)
            //  BTW, try at home (and don't take it outside!) a version
            //  without quantized probabilities... Good luck!
            foreach (var qf in from fq in freq
                               let _ = Convert.ToDecimal(fq)
                               select Quantizer(_))
            {
                fx.Add(qf);          // Quantized frequencies of letters
                Fx.Add(Fx[^1] + qf); // Cumulated quantized frequencies (led by '0'),
                                     // that is [0, 1 - p, 1 - p + 3p/4, 1] (up to quantization error)
            }
            // For those for whom ASCII is not enough
            Console.OutputEncoding = System.Text.Encoding.UTF8;

            static decimal Quantizer(decimal _x, int _precision = 12) // A dirty dozen of precision bits
            {
                var precision = 1 << _precision;
                return Math.Floor(_x * precision) / precision;
            }
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
            var width = 96;
            // A bit of an ASCII art
            Console.WriteLine("0" + new string('.', width + 1) + "1");

            foreach (char letter in text)
            {
                var index = freqMap[letter - 'a'];

                C += Fx[index] * A;
                A *= fx[index];

                // Exception handling (testing whether the interval is wide enough)
                if (A / 2m == 0m) ThrowException((C, A), text);

                // Visual representation of the coding interval
                PrintInterval(C, A);
            }
            // The ultimate code can be any number from the middle of the interval
            // (provided that the interval still has a middle! ;)
            code = C + (A / 2m);
        }
        // A fun from a few functions...
        readonly Action<(decimal, decimal), string> ThrowException = (_CA, text) =>
        {
            var (C, A) = _CA;
            var msg = $"\nMessage '{text}' contains too much information for a given arithmetic precision: ";
            msg += $"the resulting coding interval ({C}, {C + A}) is not wide enough...";
            throw new Exception(msg);
        };
        static void PrintInterval(decimal C, decimal A, int width = 96)
        {                                        // ('1 +' here and there is just for aestetics)
            var (c, a) = (Convert.ToInt32(C * width), 1 + Convert.ToInt32(A * width));
            Console.WriteLine("[" + new string(' ', c) + new string('.', a)
                                  + new string(' ', 1 + width - c - a)
                                  + $") [{C}, {C + A})");
        }

        public void Decode()
        {
            var lenght = text.Length;
            text = "";
            while(lenght-- != 0)
            {
                var index = Extract(code);

                code -= Fx[index];
                code /= fx[index];

                var letter = freqUnmap[index];
                text += Convert.ToChar(letter + 'a');
            }
        }

        public void EncodingReport()
        {
            Console.WriteLine($"\n\nIn the 8-bit ASCII code this message has {text.Length * 8} bits");
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
        private int Extract(decimal codeValue)
        {
            return Array.FindLastIndex(Fx.ToArray(), x => x < codeValue);
        }
        static void Main()
        {
            try
            {
                while (true)
                {
                    // 'Nullity' check first... Termination check second...
                    // 'EOC' stands for 'End Of Coding'.
                    if (Console.ReadLine() is string text && !text.EndsWith("EOC"))
                    {
                        using AritmeticCoder codec = new(text);
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