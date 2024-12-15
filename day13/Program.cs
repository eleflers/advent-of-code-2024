using System.Diagnostics;
using System.IO;
using MathNet.Numerics.LinearAlgebra;

namespace AOC
{
    public class ClawMachine
    {
        // Token costs
        private int costA = 3;
        private int costB = 1;
        private long Na; // Num A presses
        private long Nb; // Num B presses
        // Problem input
        public double AX; // X Dist. per A press
        public double BX; // X Dist. per B press
        public double CX; // Prize loc. X
        public double AY; // Y Dist. per A press
        public double BY; // Y Dist. per B press
        public double CY; // Prize loc. Y
        private bool part2;
        public ClawMachine(bool part2)
        {
            this.part2 = part2;
        }
        private void Solve()
        {
            // 2 simultaneous equations to solve
            //     (1) prizeX = Apresses*AbuttonX + Bpresses*BbuttonX
            //     (2) prizeY = Apresses*AbuttonY + Bpresses*BbuttonY
            // i.e. in matrix form
            //     (1) ax.na + bx.nb = cx
            //     (2) ay.na + by.nb = cy
            //Console.WriteLine("Solving claw machine for prize X:{0}, Y:{1}", CX, CY);

            // Pre-processing to check parallel lines
            // If so we need our own bespoke cost logic, if both can hit the target alone
            if (CX / AX == CY / AY && CX / BX == CY / BY)
            {
                // Both buttons can get there independently
                // Check to see which costs less
                // And also whether it can actually get there! (It may be 2.25 button presses or similar...)
                Console.WriteLine("Found machine for prize X:{0}, Y:{1} where A and B buttons can both get to the prize independently", CX, CY);
                // if (AX / costA > BX / costB)
                if (AX * costB > BX * costA && CX % AX == 0 && CY % AY == 0)
                {
                    // A is more cost effective
                    Na = (int)Math.Round(CX / AX);
                    Nb = 0;
                }
                else if (CX % BX == 0 && CY % BY == 0)
                {
                    // B is more cost effective
                    Na = 0;
                    Nb = (int)Math.Round(CX / BX);
                }
                else
                {
                    // No integer step solution for either button to get there, even though direction is right
                    // There won't be a matrix solution either so just return here
                    Na = 0;
                    Nb = 0;
                }
                Console.WriteLine("Solved NumA:{0}, NumB:{1}", Na, Nb);
                return;
            }

            // External matrix linear equation solver, from MathNet.Numerics.LinearAlgebra
            // Solve matrix equation A x = b
            var A = Matrix<double>.Build.DenseOfArray(new double[,] {
                {AX, BX},
                {AY, BY},
            });
            var b = Vector<double>.Build.Dense(new double[] {CX, CY});
            var x = A.Solve(b);
            Na = (long)Math.Round(x[0]);
            Nb = (long)Math.Round(x[1]);

            // Check for negative results, can't press a button negative times!
            if (Na < 0 || Nb < 0)
            {
                //Console.WriteLine("Discarding button press solution, negative button press required");
                Na = 0;
                Nb = 0;
            }

            //Console.WriteLine("Solved NumA:{0}, NumB:{1}", Na, Nb);

            // Arbitrarily throw out solutions requiring over 100 presses of a button, because the problem said to
            // Was just to mess with people using brute force when they got to part 2
            // Restriction is removed for part 2
            if (!part2)
            {
                if (Na > 100 || Nb > 100)
                {
                    //Console.WriteLine("Discarding button press solution, more than 100 presses of a button required");
                    Na = 0;
                    Nb = 0;
                }
            }

            // Self-check, for fractional answers, we need integers
            if (Na != 0 || Nb != 0)
            {
                if (Na * AX + Nb * BX != CX)
                {
                    //Console.WriteLine("Self-check of result failed! Prize X:{0} Y:{1} N_A:{2} N_B:{3} Expected X:{0} Got X:{4}", CX, CY, Na, Nb, Na * AX + Nb * BX);
                    //Console.WriteLine("Discarding possible fractional result");
                    Na = 0;
                    Nb = 0;
                }
                else if (Na * AY + Nb * BY != CY)
                {
                    //Console.WriteLine("Self-check of result failed! Prize X:{0} Y:{1} N_A:{2} N_B:{3} Expected Y:{1} Got Y:{4}", CX, CY, Na, Nb, Na * AY + Nb * BY);
                    //Console.WriteLine("Discarding possible fractional result");
                    Na = 0;
                    Nb = 0;
                }
            }
        }
        public long PrizeCostTokens()
        {
            Solve();
            return (long)(Na * costA + Nb * costB);
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
                Console.WriteLine("Assume default input file '{0}'", filename);
            }
            else if (argv.Count > 0)
            {
                filename = argv[0];
                Console.WriteLine("Taking CLI input file name '{0}'", filename);
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
            List<ClawMachine> clawmachinesP2 = new List<ClawMachine>();
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
                        clawmachines.Add(new ClawMachine(false));
                        clawmachinesP2.Add(new ClawMachine(true));
                        // Parse first line
                        start = line.IndexOf('X') + 1;
                        end = line.IndexOf(',');
                        clawmachines.Last().AX = int.Parse(line.Substring(start, end-start));
                        clawmachinesP2.Last().AX = int.Parse(line.Substring(start, end-start));
                        start = line.IndexOf('Y') + 1;
                        clawmachines.Last().AY = int.Parse(line.Substring(start));
                        clawmachinesP2.Last().AY = int.Parse(line.Substring(start));

                        lineCycleNo++;
                    }
                    else if (lineCycleNo == 2)
                    {
                        // Parse second line
                        start = line.IndexOf('X') + 1;
                        end = line.IndexOf(',');
                        clawmachines.Last().BX = int.Parse(line.Substring(start, end-start));
                        clawmachinesP2.Last().BX = int.Parse(line.Substring(start, end-start));
                        start = line.IndexOf('Y') + 1;
                        clawmachines.Last().BY = int.Parse(line.Substring(start));
                        clawmachinesP2.Last().BY = int.Parse(line.Substring(start));

                        lineCycleNo++;
                    }
                    else if (lineCycleNo == 3)
                    {
                        // Parse third line
                        start = line.IndexOf('X') + 2;
                        end = line.IndexOf(',');
                        clawmachines.Last().CX = int.Parse(line.Substring(start, end-start));
                        clawmachinesP2.Last().CX = int.Parse(line.Substring(start, end-start)) + 10000000000000;
                        start = line.IndexOf('Y') + 2;
                        clawmachines.Last().CY = int.Parse(line.Substring(start));
                        clawmachinesP2.Last().CY = int.Parse(line.Substring(start)) + 10000000000000;

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
            long sumP2 = clawmachinesP2.Sum(x => x.PrizeCostTokens());

            // Output
            Console.WriteLine("--");
            Console.WriteLine("Sum (P1) = {0}", sum);
            Console.WriteLine("Sum (P2) = {0}", sumP2);
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

