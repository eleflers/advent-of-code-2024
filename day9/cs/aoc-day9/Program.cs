using System;

namespace AOC
{
    public class File {
        public long Id;
        public long StartingPos;
        public long Length;
        public long Checksum()
        {
            // Checksum is the ID multiplied by the position of each block on the filesystem
            // So if file ID 8 len 3 starts at pos 2 like so ..888..
            // We need to do 2*8 + 3*8 + 4*8
            long cksum = 0;
            for (long i = 0; i < Length; i++)
            {
                cksum += Id * (StartingPos + i);
            }
            return cksum;
        }
    }

    public class FileSystem {
        public Dictionary<long,File> Files = new Dictionary<long, File>(); // Map starting pos. to File
        public Dictionary<long,long> Gaps = new Dictionary<long, long>(); // Map of starting pos. to length of gap
        public long StartNextFile = 0;
        public void AddFile(long id, long length)
        {
            File f = new File()
            {
                Id = id,
                Length = length,
                StartingPos = StartNextFile
            };
            Files.Add(StartNextFile, f);
            StartNextFile += length;
        }
        public void AddEmptySpace(long length)
        {
            Gaps[StartNextFile] = length;
            StartNextFile += length;
        }
        public long FindFirstEmptyBlock(long length, long beforeBlockPos)
        {
            foreach (var g in Gaps)
            {
                // Ordered map so we should iterate from earliest starting pos
                long gapStartBlock = g.Key;
                long gapLength = g.Value;
                if (gapLength >= length)
                {
                    // Use this one!
                    // Previous gap at X is now 0, but whatever is left is a new gap starting X + length_of_file
                    return gapStartBlock;
                }
            }
            //Console.WriteLine("No gap found, returning -1\n";
            return -1;
        }
        public void MoveFile(File f, long newStartBlockPos, bool debugprints)
        {
            // Be careful not to mix up block positions and filesystem index positions
            //if (debugprints) {
            //    Console.WriteLine("MoveFile: Received move command, file id " << f.id << " from filesystem pos " << f.startingPos << " to " << newStartBlockPos << endl;
            //}

            // Update location
            long oldStartingPos = f.StartingPos;
            f.StartingPos = newStartBlockPos;
            Files[newStartBlockPos] = f;
            Files.Remove(oldStartingPos);

            // Update gap we just filled
            long gapLength = Gaps[newStartBlockPos];
            Gaps.Remove(newStartBlockPos);
            if (gapLength - f.Length > 0)
            {
                // Gap not filled, so make new gap in remainder of space
                Gaps[newStartBlockPos + f.Length] = gapLength - f.Length;
            }

            //if (debugprints) {
            //    Console.WriteLine("  Contents of gaps map after move command:\n";
            //    for (auto it = gaps.begin(); it != gaps.end(); it++) {
            //        Console.WriteLine("    Gap pos. " << it->first << " length " << it->second << endl;
            //    }
            //}
        }
        public long ProcessFileSystem(bool debugapply)
        {
            int remaining = Files.Count();
            // Convoluted way to get a stack of positions to go through in reverse
            // since apparently maps aren't sorted
            List<long> vec = new List<long>();
            Stack<long> posStack = new Stack<long>();
            foreach (long pos in Files.Keys)
            {
                vec.Add(pos);
            }
            vec.Sort();
            foreach (var v in vec)
            {
                posStack.Push(v);
            }
            while (posStack.Count() > 0)
            {
                long pos = posStack.Pop();
                File f = Files[pos];
                if (remaining % 1000 == 0 && remaining != 0)
                {
                    Console.WriteLine("Computing possible file fragment moves, {0} files remaining...", remaining);
                }
                remaining--;
                // Run through backwards
                if (debugapply)
                {
                    Console.WriteLine("Working on file id {0}, len {1}", f.Id, f.Length);
                }
                // Find first available space big enough
                long blockPos = FindFirstEmptyBlock(f.Length, f.StartingPos);
                if (debugapply)
                {
                    Console.WriteLine("First possible space large enough (if any) at pos {0}", blockPos);
                }
                // Is it better than our current space? Update if so
                if (blockPos != -1 && blockPos < f.StartingPos)
                {
                    if (debugapply)
                    {
                        Console.WriteLine("Updating position: YES");
                    }
                    MoveFile(f, blockPos, debugapply);
                }
                else
                {
                    if (debugapply)
                    {
                        Console.WriteLine("Updating position: NO");
                    }
                }
            }
            // Checksum
            long sum = 0;
            foreach (var f in Files)
            {
                sum += f.Value.Checksum();
            }
            return sum;
        }
    }

    public class Program
    {
        public static int Main(string[] args)
        {
            Console.WriteLine("AOC Day 9 - C#");

            // For testing
            int debuglimit = 1;
            long debug = 0;
            bool debugapply = false;

            // Part 1: 6323378990915 - incorrect, too small
            // Part 1: 6323641412437 - correct

            // User args
            List<string> argv = args.ToList();
            
            string filename = "input";
            if (argv.Count() == 0)
            {
                Console.WriteLine("Assume default input file '{0}'", filename);
            }
            else if (argv.Count() > 0)
            {
                filename = argv[0];
                Console.WriteLine("Taking CLI input file name '{0}'", filename);
            }
            if (argv.Count() > 1)
            {
                Console.WriteLine("Reading 2nd input param\n    -d / --debug for debug printing");
                if (argv[1] == "-d" || argv[1] == "--debug")
                {
                    debugapply = true;
                }
                else
                {
                    debugapply = false;
                }
            }
            if (debugapply)
            {
                Console.WriteLine("--\nDEBUG MODE : ON\nLINE LIMIT : {0}\n--", debuglimit);
            }

            // Variables for output
            //long sum = 0;

            // Read file
            String? line;
            // Object approach
            FileSystem fsP1 = new FileSystem()
            {
                StartNextFile = 0
            };
            FileSystem fsP2 = new FileSystem()
            {
                StartNextFile = 0
            };

            using (var streamReader = new StreamReader(filename))
            {
                while ((line = streamReader.ReadLine()) != null)
                {
                    debug++;
                    if (debugapply)
                    {
                        Console.WriteLine(line);
                    }
                    // Input is a single line (disk map), a series of individual blocks
                    // We need to iterate over the line one by one
                    // 1st digit is a file length
                    // 2nd digit is a free space length
                    // Repeats until end of line
                    // Each file has a unique ID starting with 0 (these are unrelated to the block size! Always 1)
                    int pos = 0;
                    long len;
                    long id = 0; // ID of file starting at 0, incrementing
                    while (pos < line.Length)
                    {
                        // Read file length
                        len = Int64.Parse(line.Substring(pos,1));
                        //Console.WriteLine("Parsed number {0} from string '{1}'", len, line.Substring(pos,1));
                        // Add file 'ID' to memory 'len' times
                        for (long i = 0; i < len; i++)
                        {
                            // Use P2 code to solve P1 also, just add 'len' files of length 1 instead of 1 file length 'len'
                            fsP1.AddFile(id, 1);
                        }
                        // Filesystem for P2 also, single file length 'len'
                        fsP2.AddFile(id, len);
                        // Incremenent ID
                        id++;
                        //Console.WriteLine("ID incremented to " << id << endl;
                        pos++;
                        
                        // Read free space
                        if (pos < line.Length)
                        {
                            len = Int64.Parse(line.Substring(pos,1));
                            //Console.WriteLine("Parsed number {0} from string '{1}'", len, line.Substring(pos,1));
                            // Add free space '.' 'len' times
                            fsP1.AddEmptySpace(len);
                            fsP2.AddEmptySpace(len);
                            pos++;
                        }
                    }
                }
            }

            Console.WriteLine("== Part 1 ==");

            // Processing P1
            // Refactored to use same code as P2, each file is just broken up into files of blocklength 1 with the same ID
            // Only try to move a file once before moving on
            // Duplicate into a stack just so we can easily go backwards over each file exactly once
            // Even if we're moving them around in the actual FileSystem obj.
            long sumP1 = fsP1.ProcessFileSystem(debugapply);

            Console.WriteLine("== Part 2 ==");
            long sumP2 = fsP2.ProcessFileSystem(debugapply);

            // Output
            Console.WriteLine("P1 Checksum = {0}", sumP1);
            Console.WriteLine("P2 Checksum = {0}", sumP2);

            Console.WriteLine("--\nEnd.");
            return 0;
        }
    }
}

