using System.Diagnostics;
using System.IO;
using System.Runtime.CompilerServices;
using System.Text.RegularExpressions;

namespace AOC
{
    public class Robot
    {
        public int x;
        public int y;
        public int vx;
        public int vy;
        //private const int sizeX = 11; // Example grid
        //private const int sizeY = 7; // Example grid
        private const int sizeX = 101;
        private const int sizeY = 103;
        public int quadrant = -1;
        public Robot(int px, int py, int vx, int vy)
        {
            this.x = px;
            this.y = py;
            this.vx = vx;
            this.vy = vy;
        }
        // Move robot N times
        public void Move(int N)
        {
            // Displacement
            int newX = x + (N * vx);
            int newY = y + (N * vy);
            // Wrap around like pacman
            while (newX < 0)
            {
                newX += sizeX;
            }
            while (newY < 0)
            {
                newY += sizeY;
            }
            x = newX % sizeX;
            y = newY % sizeY;
            // Calculate quadrant
            UpdateQuadrant();
        }
        private void UpdateQuadrant()
        {
            // Divide map into 4 quadrants
            // We are _excluding_ the central line of X's
            // .....|X|.....
            // ..1..|X|..2..
            // .....|X|.....
            // -----+X+-----
            // XXXXXXXXXXXXX
            // -----+X+-----
            // .....|X|.....
            // ..3..|X|..4..
            // .....|X|.....
            int halfX = (sizeX - 1) / 2;
            int halfY = (sizeY - 1) / 2;

            // We need all the conditions because we are exclusing the central positions
            if (x < halfX)
            {
                if (y < halfY)
                    quadrant = 1;
                else if (y > halfY)
                    quadrant = 3;
            }
            else if (x > halfX)
            {
                if (y < halfY)
                    quadrant = 2;
                else if (y > halfY)
                    quadrant = 4;
            }
        }
        public bool IsInQuadrant(int q)
        {
            return q == quadrant;
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
            int seconds = 100;
            Console.WriteLine("Moving robots {0} times...", seconds);
            robots.ForEach(x => x.Move(100));
            // Get quadrant count
            // Safety factor is no. in each quadrant multiplied
            sum = robots.Sum(x => x.IsInQuadrant(1) ? 1 : 0)
                * robots.Sum(x => x.IsInQuadrant(2) ? 1 : 0)
                * robots.Sum(x => x.IsInQuadrant(3) ? 1 : 0)
                * robots.Sum(x => x.IsInQuadrant(4) ? 1 : 0);

            if (debugmode)
            {
                Console.WriteLine("Robot 1 is at x,y [{0},{1}], quadrant {2}", robots.First().x, robots.First().y, robots.First().quadrant);
            }

            // Output
            Console.WriteLine("--");
            Console.WriteLine("Safety factor (P1) = {0}", sum);
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

