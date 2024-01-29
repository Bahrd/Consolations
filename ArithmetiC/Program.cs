namespace ArithmeticCoding
{
    class AritmeticCoder : IDisposable
    {
        string text;
        decimal code;
        decimal C, A;
        readonly List<decimal> fx = new(), Fx = new() { 0m };

        /**
         * "PMS"
         * Taken from nowhere...
         */
        const int size = 3;
        const double p = 1 / 4.0;
        const double pa = 1.0 - p, pb = 3*p/4, pc = p/4;

        readonly int[] letterFreqMap = new int[size];
        readonly int[] letterFreqUnmap = new int[size] { 0, 1, 2 
                                                        /*,
                                                        2	,
                                                        3  ,
                                                        4  ,
                                                        5  ,
                                                        6  ,
                                                        7*/
                                                       };

        readonly double[] letterFreq = new double[size] { pa, pb, pc
                                                        /*,
                                                           1.0/64.0
                                                        0.000488281,
                                                        0.000976563,
                                                        0.001953125,
                                                        0.00390625,
                                                        0.0078125,
                                                        0.015625,
                                                        0.03125,
                                                        0.937988281*/
                                                        /*0.000976563,
                                                    0.001953125,
                                                    0.00390625,
                                                    0.0078125,
                                                    0.015625,
                                                    0.03125,
                                                    0.0625,
                                                    0.875976563*/
                                                        };
        #region frequencies
        /**
         * "Etaoin Shrdlu"
         * Taken from http://rinkworks.com/words/letterfreq.shtml
         */
        /*      const int size = 26;

              int[] letterFreqMap = new int[size];
              int[] letterFreqUnmap = new int[size]    { 
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


              double[] letterFreq = new double[size] 
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
            (text, code, C, A) = (args, 0.0m, 0.0m, 1.0m);

            //  All kinds of for-loops I know...
            //  An old school...
            for (var index = 0; index < size; ++index)
                letterFreqMap[letterFreqUnmap[index]] = index;

            //  ... vs. LINQ style
            foreach (var fq in from _freq in letterFreq
                               let freq = Convert.ToDecimal(_freq)
                               let precision = (1 << 12) // A dozen of precision bits
                               select Math.Floor(freq * precision) / precision)
            {
                fx.Add(fq);          // Frequencies: eg. [p, p + 3p/4, p + 1p/4].
                Fx.Add(Fx[^1] + fq); // Cumulated frequencies: eg. [0, 1 - p, 1 - p + 3p/4].
                                     // The last one is 1.0 and not included in the array.
            }
            Console.OutputEncoding = System.Text.Encoding.UTF8;
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
                var index = letterFreqMap[letter - 'a'];

                C += Fx[index] * A;
                A *= fx[index];
                
                // Exception handling
                if(A / 2m == 0m)
                {
                    var msg =  $"\nMessage '{text}' has too much information for a given arithmetic precision.";
                        msg += $"Coding interval ({C}, {C + A}) not wide enough...";
                    throw new Exception(msg);
                }

                // Visual representation of the coding interval
                var (c, a) = (Convert.ToInt32(C * width), 1 + Convert.ToInt32(A * width));
                Console.WriteLine("[" + new string(' ', c) + new string('.', a)
                                      + new string(' ', width + 1 - c - a)
                                      + $") [{C}, {C + A})");
            }
            // The final code is just any number from the middle of the interval
            // (provided that the interval still has a middle! ;)
            code = C + (A/2m);
        }

        public void Decode()
        {
            var lenght = text.Length;
            text = "";
            for (var _ = 0; _ < lenght; ++_)
            {
                var index = Extract(code);

                code -= Fx[index];
                code /= fx[index];

                var letter = letterFreqUnmap[index];
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
                    // 'Nullity' check first...
                    if (Console.ReadLine() is string text && text != "quit")
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
                Console.Error.WriteLine($"\nExpected exception has occured: {e.Message}\n");
            }
        }
    }
}