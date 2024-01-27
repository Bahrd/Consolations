namespace ArithmeticCoding
{
    class AritmeticCoder : IDisposable
    {
        private readonly List<decimal> fx, Fx;
        string text;
        decimal code;
        decimal C, A;

        #region frequencies
        /**
         * "PMS"
         * Taken from nowhere...
         */
        const int size = 2;
        const double p = 1.0/64.0;
        readonly int[] letterFreqMap = new int[size];
        readonly int[] letterFreqUnmap = new int[size] {
                                                0   ,
                                                1   /*,
                                                2	,
                                                3  ,
                                                4  ,
                                                5  ,
                                                6  ,
                                                7*/
                                               };

        readonly double[] letterFreq = new double[size]
                                                {
                                                       1.0 - p,
                                                       p
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
        public AritmeticCoder(string ?args)
        {
            text = args ?? "abba";
            code = 0.0m;
            C = 0.0m;
            A = 1.0m;

            for (int index = 0; index < size; ++index)
            {
                letterFreqMap[letterFreqUnmap[index]] = index;
            }

            fx = new List<decimal>(size);
            Fx = new List<decimal>(size);

            foreach (double _freq in letterFreq)
            {
                // conversion (trimming) to finite binary representation
                int bits = 12;
                decimal freq = Convert.ToDecimal(_freq);
                decimal precision = (1 << bits);

                fx.Add(Convert.ToDecimal(Math.Floor(freq * precision) / precision));
            }
            // computations of cummulative frequencies
            Fx.Add(0.0m);
            for (int i = 1; i < fx.Capacity; ++i)
            {
                Fx.Add(Fx[i - 1] + fx[i - 1]);
            }
        }
        ~AritmeticCoder()
        {
            Dispose();
        }
        public void Dispose()
        {
            Console.Out.WriteLine("Aftermath!");
            GC.SuppressFinalize(this);
        }
        public void Encode()
        {
            foreach (char letter in text)
            {
                int index = letterFreqMap[letter - 'a'];

                C += Fx[index] * A;
                A *= fx[index];

                Console.Out.WriteLine($"[{C}, {C + A})");
            }
            if (A == 0m)
            {
                throw new Exception($"Message '{text}' too long for a given arithmetic precision.");
            }
            code = C + A / 2m;
            /// "Binarization"
            {
                int mb = Convert.ToInt32(Math.Ceiling(-Math.Log(Convert.ToDouble(A), 2))) + 1;

                Decimal binaryRange = Convert.ToDecimal(1 << mb);
                Decimal coded_message = Math.Floor(code * binaryRange) / binaryRange;
                code = coded_message;
            }
        }

        public void Decode()
        {
            int length = text.Length;
            text = "";

            for (int i = 0; i < length; ++i)
            {
                int index = Extract(code);

                code -= Fx[index];
                code /= fx[index];

                int letter = letterFreqUnmap[index];
                text += Convert.ToChar(letter + 'a');
            }
        }

        public void EncodingReport()
        {
            Console.Out.WriteLine($"\n\nIn the 8-bit ASCII code this message has {text.Length * 8} bits");
            int bits_size = Convert.ToInt32(Math.Ceiling(Math.Log(size, 2)));
            Console.Out.WriteLine($"\n\nIn the {bits_size}-bit code this message has {text.Length * bits_size} bits");
            Console.Out.WriteLine($"The arithmetic code of \n\n{text}\n\nis any number from interval\n[{C}, {C + A})");
            Console.Out.WriteLine($"\n\nFor instance\n{code}\n");
            int messageLength = Convert.ToInt32(Math.Ceiling(-Math.Log(Convert.ToDouble(A), 2)));
            Console.Out.WriteLine($"Which binary representation needs 'only' (wait for it...) {messageLength} bits\n");
        }
        public void DecodingReport()
        {
            Console.Out.WriteLine($"\n\nThe decoded message is:\n{text}\n");
        }
        private int Extract(decimal codeValue)
        {
            for (int i = Fx.Capacity - 1; i > 0; --i)
            {
                if (codeValue >= Fx[i])
                {
                    return i;
                }
            }
            return 0;
        }
        static void Main(string[] args)
        {
            try
            {
                while (true)
                {
                    string ?text = Console.ReadLine();
                    if (text == "quit")
                    {
                        break;
                    }
                    using AritmeticCoder codec = new(text);
                    codec.Encode();
                    codec.EncodingReport();

                    codec.Decode();
                    codec.DecodingReport();
                }
            }
            catch (Exception e)
            {
                Console.Error.WriteLine($"\nExpected exception has occured because: {e.Message}\n");
            }
        }
    }
}