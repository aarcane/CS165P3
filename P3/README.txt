This was a particularly rough assignment due to a little too much procrastination on my behalf.  The material in and of itself wasn't difficult, but I mistakenly believed I had until FRIDAY to submit when in actuality, the duedate was WEDNESDAY.  I was planning to do most of the work THURSDAY, and screwed myself out of some serious optimization and finishing time.  That said, I present to you my /masterpiece/

Huffman Coding is implemented in the manner prescribed.  It should have no gross inefficiencies, and as seen in the table included, performs sufficiently well.  Strangely, encoding is faster than decoding.  This is because of the treeMap lookup on std::vector<bool> taking significant numbers of compares compared to the treeMap lookup on unsigned char going the other way.
Internally, Huffman coding uses primarily std::vector<bool> for it's convenient interface and ease of use.  A few additional vector insertion and extraction operators were added to perform bitwise extraction and insertion operations, and the whole shindig is piped over to a vector<unsigned char> for subsequent writing.  Vector is nice, if you hadn't realized, you can pretty much access it just like an array or a pointer with minimal effort.  C++11 added some new stuff, too.
Anyway, building the tree and map is an O(1) operation on either end, but filling the charCount array (from which to build the tree) is an O(N) operation.  Tree can take at most 256 leaves, which means at most 255 internal nodes, for 512 nodes.  Building the maps are also bounded by similarly rigid upper bounds, equally low.  As these pale in comparison to the actual file processing time, it can be safely ignored.  The dominant factor is the lookup of EVERY bit sequence in the lookup table.  This could be, with minor tweaks, augmented to skip any bit pattern too short to match and error on any pattern too long to succeed, or it could be filtered through a bloom-like pre-filter on bit-pattern length.  It would require numerous, but small, changes.  I didn't have the time.

Ultimately, the fewer characters in the dictionary, the fewer lookups will need to be performed, especially on particularly long bit patterns, and the faster decoding will be.

Lempel Ziv coding.

The LZ77 algorithm core ideals were implemented succinctly and quickly.  I was able to identify proper sequences to output and proper previus strings to match without issue, and even verified this by forcing the lz77 algorithm to run "in place" on the data, printing the "post compressed" data to screen as it went.  It was fast.  Smaller dictionary was faster than big.
for speed's sake I chose to optimize lookup times over memory consumption, and it paid off.  The standard substring algorithm is O(NM) complexity (or O(16*window) in our case), which is quite high.  The STL provides std::multimap.  I opted to use this hash table implementation to provide rapid lookups of previous substrings up to window size.  This took a little tweaking and a little coaxing, but it ultimately worked out well.  I believe some more memory efficient algorithms could be used if I had more time to tweak this portion of the algirithm, but ultimately insertion and deletion are O(1) operations, so a moot point.  The only problem here is that with small windows and large files, the lookup table quickly started returning "expired" results.  I mitigated this some by limiting the size of entries stored in the lookup table, and also by pruning expired results as I encountered them.

Unfortunately, due to some imperfections in bit encoding that I haven't the time to work out, I am unable to expand LZ1 and LZ2 coding at this time, so I've been unable to time it.  That said, just studying the algorithm, it appears to be both MUCH faster and MUCH more memory efficient than the compression algorithm.

I appologize for the incomplete nature of this work, and hope that you can see to reading the code and at least verifying that the core compression algorithm is working (though I am unsure about the output portion.  Bits and whatnot.).


Further analysis is somewhat moot, as the algorithms ALL very greatly in terms of both input size and input alphabet size.

As the table shows, however, the infamous LZ4 algorithm was able to dominate over either of these algorithms in both compression and decompression time while performing on par with them in terms of compression size.  

Included are some additional test files that I used but that are not a part of the assignment.
