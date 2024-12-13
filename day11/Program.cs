using System.Diagnostics;
using System.IO;
using System.Runtime.CompilerServices;

namespace AOC
{
    public class Stone
    {
        private long engraving;
        // Recursively keep track of any child stones on the left, should this one split
        private List<Stone> childStones = new List<Stone>();
        public Stone(long initialEngraving)
        {
            engraving = initialEngraving;
        }
        public long EngravingSum
        {
            get => engraving + childStones.Sum(x => x.EngravingSum);
        }
        public int StoneCount
        {
            get => 1 + childStones.Sum(x => x.StoneCount);
        }
        public void Blink()
        {
            // Call recursively on any children
            childStones.ForEach(x => x.Blink());

            // "The rules" are applied every time we blink...
            // 1st applicable rule applied
            // - engr. == 0 -> 1
            // - Even no. digits -> Duplicates
            //     - Left half digits -> Left stone
            //     - Right half digits -> Right stone
            //     - (Ignore leading 0s, 1000 -> 10,0)
            // - engr. -> engr. * 2024
            int noDigits = (int)Math.Floor(Math.Log10(engraving)) + 1;
            //Console.Write("Stone {0} -> ", engraving);
            if (engraving == 0)
            {
                // 0 -> 1
                engraving = 1;
                //Console.WriteLine("{0}", 1);
            }
            else if (noDigits % 2 == 0)
            {
                //Console.WriteLine("{0} is a {1} digit number, duplicating stone...", engraving, noDigits);
                // Duplicate
                // Split no. in 2
                // E.g. 1234 -> 12, 34
                int digitsPerStone = noDigits / 2;
                int powerOfTen = (int)Math.Pow(10, digitsPerStone);
                long leftStone = (long)Math.Floor((double)engraving / powerOfTen);
                long rightStone = engraving - (leftStone * powerOfTen);

                //Console.WriteLine("Calculated left no = {0}, right no = {1}", leftStone, rightStone);

                // Make a child stone obj. to track the stone on the left
                childStones.Add(new Stone(leftStone));
                engraving = rightStone;
                //Console.WriteLine("{0} & {1}", leftStone, rightStone);
            }
            else
            {
                engraving *= 2024;
                //Console.WriteLine("{0}", engraving);
            }
        }
    }
    public class Program
    {
        public static int Main(string[] args)
        {
            Console.WriteLine("--\nAoC Day 11\n--");
            // For testing
            int debuglimit = 1;
            bool debugmode = false;

            // User args
            List<string> argv = args.ToList();
            string filename = "input";
            if (argv.Count == 0)
            {
                Console.WriteLine("Assume default input file '{0}", filename);
            }
            else if (argv.Count > 0)
            {
                filename = argv[0];
                Console.WriteLine("Taking CLI input file name {0}'", filename);
            }
            if (argv.Count > 1)
            {
                Console.WriteLine("Reading 2nd input param\n    -d / --debug for debug printing");
                if (argv[1] == "-d" || argv[1] == "--debug")
                {
                    debugmode = true;
                }
                else
                {
                    debugmode = false;
                }
            }
            if (debugmode)
            {
                Console.WriteLine("--\nDEBUG MODE : ON\nLINE LIMIT : {0}", debuglimit);
            }

            // Variables for output
            long sum = 0;

            // Read file
            String? line;
            int lineNr = 0;
            List<Stone> stones = new List<Stone>();
            using (var streamReader = new StreamReader(filename))
            {
                while ((line = streamReader.ReadLine()) != null && (!debugmode || lineNr < debuglimit))
                {
                    if (debugmode) {
                        Console.WriteLine(line);
                    }

                    // Parse input numbers, delimited by spaces
                    List<int> numbers = line.Split(' ').ToList().Select(x => int.Parse(x)).ToList<int>();
                    stones = numbers.Select(x => new Stone(x)).ToList();
                    Console.WriteLine("Starting with {0} stones", stones.Count);

                    lineNr++;
                }
            }

            // Processing
            // Run iterations of the rules on stones
            int blinks = 25;
            //int blinks = 6;
            for (int i = 0; i < blinks; i++)
            {
                if (debugmode)
                {
                    Console.WriteLine("About to apply blink {0} to all stones...", i+1);
                }
                stones.ForEach(x => x.Blink());
                if (debugmode)
                {
                    Console.WriteLine("After {0} blinks, there are {1} stones\n--", i+1, stones.Sum(x => x.StoneCount));
                }
            }
            sum = stones.Sum(x => x.StoneCount);

            // Output
            Console.WriteLine("--");
            Console.WriteLine("Number of stones = {0}", sum);
            if (sum == 55312)
            {
                Console.WriteLine("Answer matches example expected answer");
            }
            else
            {
                Console.WriteLine("Answer does not match example expected answer");
            }

            Console.WriteLine("--\nEnd.");
            return 0;
        }
    }
}

