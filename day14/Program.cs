using System.Diagnostics;
using System.IO;
using System.Text.RegularExpressions;

namespace AOC
{
    public class Robot
    {
        public int px;
        public int py;
        public int vx;
        public int vy;
        public Robot(int px, int py, int vx, int vy)
        {
            this.px = px;
            this.py = py;
            this.vx = vx;
            this.vy = vy;

        }
    }
    public class Program
    {
        public static int Main(string[] args)
        {
            Console.WriteLine("--\nAoC Day 14\n--");
            // For testing
            int debuglimit = 12;
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
            List<Robot> robots = new List<Robot>();
            using (var streamReader = new StreamReader(filename))
            {
                while ((line = streamReader.ReadLine()) != null && (!debugmode || lineNr < debuglimit))
                {
                    if (debugmode)
                    {
                        Console.WriteLine(line);
                    }

                    // Parse robots
                    var r = new Regex(@"p=(\S+),(\S+) v=(\S+),(\S+)");
                    var m = r.Match(line);
                    if (debugmode)
                    {
                        Console.WriteLine("Match groups '{0}', p={1},{2} v={3},{4}", m.Groups.Count, m.Groups[1].Value, m.Groups[2].Value, m.Groups[3].Value, m.Groups[4].Value);
                    }
                    if (m.Success && m.Groups.Count == 5)
                    {
                        // Messy regex parse method
                        robots.Add(
                            new Robot(
                                int.Parse(m.Groups[1].Value),
                                int.Parse(m.Groups[2].Value),
                                int.Parse(m.Groups[3].Value),
                                int.Parse(m.Groups[4].Value)));
                    }
                    else
                    {
                        throw new InvalidDataException("Fix regex input!");
                    }

                    lineNr++;
                }
            }

            // Processing
            if (debugmode)
            {
                Console.WriteLine("Made {0} robots", robots.Count);
            }

            // Output
            Console.WriteLine("--");
            Console.WriteLine("Sum = {0}", sum);
            if (sum == 12)
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

