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
        private const int halfX = (sizeX - 1) / 2;
        private const int halfY = (sizeY - 1) / 2;
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
            // Wrap around like pacman, negative values add size as many times as needed
            if (newX < 0)
            {
                newX = newX + sizeX * ((int)Math.Floor( - newX / (double)sizeX) + 1);
            }
            if (newY < 0)
            {
                newY = newY + sizeY * ((int)Math.Floor( - newY / (double)sizeY) + 1);
            }
            // Wrap around positive values too
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
        public double DistanceToCentre
        {
            // x^2 + y^2 distance to centre, no point normalising
            get => Math.Pow(x - halfX, 2) + Math.Pow(y - halfY, 2);
        }
    }
    public class Bathroom
    {
        private List<Robot> robots;
        private const int sizeX = 101;
        private const int sizeY = 103;
        public int secondsPassed = 0;
        private string emptyRow = new string('.', sizeX);
        private List<char[]> grid = new List<char[]>();
        public int SafetyScore
        {
            get
            {
                return robots.Sum(x => x.IsInQuadrant(1) ? 1 : 0)
                    * robots.Sum(x => x.IsInQuadrant(2) ? 1 : 0)
                    * robots.Sum(x => x.IsInQuadrant(3) ? 1 : 0)
                    * robots.Sum(x => x.IsInQuadrant(4) ? 1 : 0);
            }
        }
        public double AverageDistanceToCentre
        {
            get
            {
                // Sum of distance to centre, divided by no. of robots, but no point normalising
                // since count of robots does not change
                return robots.Sum(x => x.DistanceToCentre);
            }
        }
        public Bathroom(List<Robot> robots)
        {
            this.robots = robots;
        }
        public void MoveRobots(int N)
        {
            robots.ForEach(x => x.Move(N));
            secondsPassed += N;
        }
        public void Draw()
        {
            // Draw the bathroom on screen
            Console.WriteLine("Drawing bathroom:");

            // Blank map
            grid = new List<char[]>();
            for (int i = 0; i < sizeY; i++)
            {
                grid.Add(emptyRow.ToCharArray());
            }
            // Place every robot in array
            foreach (Robot r in robots)
            {
                grid[r.y][r.x] = '#';
            }

            // Draw to screen
            foreach (var line in grid)
            {
                Console.WriteLine(line);
            }
            Console.WriteLine("{0} seconds (robot moves) have passed", secondsPassed);
            Console.WriteLine("Safety score is {0}", SafetyScore);
            Console.WriteLine("Average Distance is {0}", AverageDistanceToCentre);
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

            Bathroom bathroom = new Bathroom(robots);

            // Processing
            if (debugmode)
            {
                Console.WriteLine("Made {0} robots", robots.Count);
            }
            int seconds = 100;
            Console.WriteLine("Moving robots {0} times...", seconds);
            bathroom.MoveRobots(seconds);
            // Get quadrant count
            // Safety factor is no. in each quadrant multiplied
            sum = bathroom.SafetyScore;

            // Undo Part 1
            bathroom.MoveRobots(-seconds);

            // Part 2
            bathroom.Draw();
            long lowestSafetyScore = bathroom.SafetyScore;
            double lowestDistanceScore = bathroom.AverageDistanceToCentre;
            seconds = 0;
            while (seconds < 10000)
            {
                bathroom.MoveRobots(1);
                seconds++;
                if (bathroom.AverageDistanceToCentre < lowestDistanceScore)
                {
                    lowestDistanceScore = bathroom.AverageDistanceToCentre;
                    bathroom.Draw();
                }
                //Thread.Sleep(200);
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

