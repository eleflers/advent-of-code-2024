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

            List<long> result = SimulateBlink(engraving);

            if (result.Count == 1)
            {
                engraving = result[0];
            }
            else
            {
                childStones.Add(new Stone(result[0]));
                engraving = result[1];
            }
        }
        // Simulate a blink without actually making stone objects recursively
        public static List<long> SimulateBlink(long engraving)
        {
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
                return new List<long>(){1};
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
                return new List<long>(){leftStone, rightStone};
                //Console.WriteLine("{0} & {1}", leftStone, rightStone);
            }
            else
            {
                return new List<long>(){engraving * 2024};
                //Console.WriteLine("{0}", engraving);
            }
        }
    }
    public class StoneLine
    {
        // Dictionary of the number engraved on a stone, to the number of times it occurs in the line
        public Dictionary<long,long> Stones = new Dictionary<long, long>();
        public long StoneCount
        {
            get => Stones.Sum(x => x.Value);
        }
        public StoneLine(List<int> startingNumbers)
        {
            foreach (var s in startingNumbers)
            {
                if (Stones.ContainsKey(s))
                {
                    Stones[s] += 1;
                }
                else
                {
                    Stones.Add(s, 1);
                }
            }
        }
        public void Blink(int blinkcount)
        {
            for (int i = 0; i < blinkcount; i++)
            {
                if (i+1 % 10 == 0)
                {
                    Console.WriteLine("Blink {0}/{1}", i+1, blinkcount);
                }
                Blink();
            }
        }
        public void Blink()
        {
            // Make a new dictionary and swap over, so we aren't confusing stone counts from
            // before and after blink in case they collide
            Dictionary<long,long> result = new Dictionary<long, long>();
            long count;
            foreach (long engr in Stones.Keys)
            {
                count = Stones[engr];
                var after = Stone.SimulateBlink(engr);
                foreach (long newEngr in after)
                {
                    if (result.ContainsKey(newEngr))
                    {
                        result[newEngr] += count;
                    }
                    else
                    {
                        result.Add(newEngr, count);
                    }
                }
            }
            // Swap
            Stones = result;
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
            bool disableP1 = true;

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
            StoneLine stoneline = null;
            using (var streamReader = new StreamReader(filename))
            {
                while ((line = streamReader.ReadLine()) != null && (!debugmode || lineNr < debuglimit))
                {
                    if (debugmode) {
                        Console.WriteLine(line);
                    }

                    // Parse input numbers, delimited by spaces
                    Console.WriteLine("Parsing input...");
                    List<int> numbers = line.Split(' ').ToList().Select(x => int.Parse(x)).ToList<int>();
                    if (!disableP1)
                    {
                        // For large test data don't bother with part 1 method
                        stones = numbers.Select(x => new Stone(x)).ToList();
                    }
                    stoneline = new StoneLine(numbers); // Part 2 optimisations
                    Console.WriteLine("Starting with {0} stones", stones.Count);

                    lineNr++;
                }
            }

            // Processing
            // Run iterations of the rules on stones
            int blinks = 25;
            if (!disableP1)
            {
                Console.WriteLine("Start Part 1, {0} iterations...", blinks);
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
            }

            // For part 2, blinks 75, we need to be a little smarter
            blinks = 75;
            Console.WriteLine("Start Part 2, {0} iterations...", blinks);
            stoneline.Blink(blinks);
            long sumP2 = stoneline.StoneCount;

            // Output
            Console.WriteLine("--");
            Console.WriteLine("P1 number of stones = {0}", sum);
            Console.WriteLine("P2 number of stones = {0}", sumP2);
            if (sum == 55312 || sum == 216996)
            {
                Console.WriteLine("P1 answer matches example expected answer");
            }
            else
            {
                Console.WriteLine("P1 answer does not match example expected answer");
            }

            Console.WriteLine("--\nEnd.");
            return 0;
        }
    }
}

