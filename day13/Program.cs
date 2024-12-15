using System.Diagnostics;
using System.IO;

namespace AOC
{
    public class ClawMachine
    {
        // Token costs
        private int costA = 3;
        private int costB = 1;
        private int Na; // Num A presses
        private int Nb; // Num B presses
        // Problem input
        public int AX; // X Dist. per A press
        public int BX; // X Dist. per B press
        public int CX; // Prize loc. X
        public int AY; // Y Dist. per A press
        public int BY; // Y Dist. per B press
        public int CY; // Prize loc. Y
        public ClawMachine()
        {
        }
        private void Solve()
        {
            // 2 simultaneous equations to solve
            //     (1) prizeX = Apresses*AbuttonX + Bpresses*BbuttonX
            //     (2) prizeY = Apresses*AbuttonY + Bpresses*BbuttonY
            // aka
            //     (1) cx = ax.na + bx.nb
            //     (2) cy = ay.na + by.nb
            // Rearrange (1) for nb
            //     (1a) bx.nb = cx - ax.na
            //     (1a) nb    = (cx - ax.na)/bx
            // Subst. (1a) -> (2)
            //     (3) cy = ay.na + by.(cx - ax.na)/bx
            //     (3) cy = ay.na + (by/bx)cx - (by/bx)ax.na
            //     (3) cy = na(ay - (by/bx)ax) + (by/bx)cx
            //     (3) na = (cy - (by/bx)cx) / (ay - (by/bx)ax)
            Console.WriteLine("Solving claw machine for prize X:{0}, Y:{1}", CX, CY);

            // First check some conditions for no solution
            if (BX == 0 || (AY - (BY/BX) * AX) == 0)
            {
                // Divide by 0 is required!
                Na = 0;
                Nb = 0;
                Console.WriteLine("Avoiding a divide by 0, no solutions");
                return;
            }
            // Equation (3)
            Na = (CY - (BY/BX) * CX) / (AY - (BY/BX) * AX);
            // Equation (1a)
            Nb = (CX - AX * Na) / BX;

            // Check for negative results, can't press a button negative times!
            if (Na < 0 || Nb < 0)
            {
                Console.WriteLine("Avoiding negative button press solution");
                Na = 0;
                Nb = 0;
            }

            Console.WriteLine("Solved NumA:{0}, NumB:{0}", Na, Nb);

            // Arbitrarily throw out solutions requiring over 100 presses of a button, because the problem said so
            if (Na > 100 || Nb > 100)
            {
                Console.WriteLine("Discarding button press solution, more than 100 presses of a button required");
                Na = 0;
                Nb = 0;
            }

        }
        public int PrizeCostTokens()
        {
            Solve();
            return Na * costA + Nb * costB;
        }
    }
    public class Program
    {
        public static int Main(string[] args)
        {
            Console.WriteLine("--\nAoC Day 13\n--");
            // For testing
            int debuglimit = 3;
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
            List<ClawMachine> clawmachines = new List<ClawMachine>();
            using (var streamReader = new StreamReader(filename))
            {
                int lineCycleNo = 1;
                int start;
                int end;
                while ((line = streamReader.ReadLine()) != null && (!debugmode || lineNr < debuglimit))
                {
                    if (debugmode) {
                        Console.WriteLine(line);
                    }

                    // Input in form:
                    // Button A: X+94, Y+34
                    // Button B: X+22, Y+67
                    // Prize: X=8400, Y=5400
                    //
                    if (lineCycleNo == 1)
                    {
                        clawmachines.Add(new ClawMachine());
                        // Parse first line
                        start = line.IndexOf('X') + 1;
                        end = line.IndexOf(',');
                        clawmachines.Last().AX = int.Parse(line.Substring(start, end-start));
                        start = line.IndexOf('Y') + 1;
                        clawmachines.Last().AY = int.Parse(line.Substring(start));

                        lineCycleNo++;
                    }
                    else if (lineCycleNo == 2)
                    {
                        // Parse second line
                        start = line.IndexOf('X') + 1;
                        end = line.IndexOf(',');
                        clawmachines.Last().BX = int.Parse(line.Substring(start, end-start));
                        start = line.IndexOf('Y') + 1;
                        clawmachines.Last().BY = int.Parse(line.Substring(start));

                        lineCycleNo++;
                    }
                    else if (lineCycleNo == 3)
                    {
                        // Parse third line
                        start = line.IndexOf('X') + 2;
                        end = line.IndexOf(',');
                        clawmachines.Last().CX = int.Parse(line.Substring(start, end-start));
                        start = line.IndexOf('Y') + 2;
                        clawmachines.Last().CY = int.Parse(line.Substring(start));

                        lineCycleNo++;
                    }
                    else if (string.IsNullOrWhiteSpace(line))
                    {
                        // Blank line between clawmachines
                        lineCycleNo = 1;
                    }
                    else
                    {
                        throw new FileLoadException("Parsing file went wrong, check file contents!");
                    }

                    lineNr++;
                }
            }

            // Processing
            if (debugmode)
            {
                Console.WriteLine("Parsed {0} Claw Machine objects", clawmachines.Count);
                //var cm = clawmachines.First();
                //Console.WriteLine("Printing contents of first machine:\n{0}\n{1}\n{2}\n{3}\n{4}\n{5}", cm.AbuttonX, cm.AbuttonY, cm.BbuttonX, cm.BbuttonY, cm.prizeX, cm.prizeY);
            }
            sum = clawmachines.Sum(x => x.PrizeCostTokens());

            // Output
            Console.WriteLine("--");
            Console.WriteLine("Sum = {0}", sum);
            if (sum == 480)
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

