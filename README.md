# Turtledeflate

Turtledeflate is a slow deflate format data compressor.

If you want to impress your friends with small calgary, cantrbry or silesia .gz files then Turtledeflate is for you!

All you need is patience for it to finish compressing.

It has been inspired by the deflate performance from kzip by Ken Silverman.

## Comparison to Zopfli

Zopfli only does path tracing with iterative optimizations and some basic block splitting ?

To squish more bits out of the deflate format exact block boundaries are essential. Turtledeflate does multiple block splitter steps with different path tracing approaches to reach an optimal block size for the final compression step. Then at the final compression step the near optimal blocks are compressed by some slightly randomizing path tracing, keeping the best result. The randomizing works by starting with a certain bit precision in fixed point and increasing the precision in steps, then trying to push the resulting high precision pathes out if their respective local minima.

This makes Turtledeflate very slow but slightly efficient than Zopfli on the tests I have run.


## Some performance numbers

I hope these are correct.

> Zopfli had the example app defaults of 1MB superblock, 512 blocks max and "--gzip --i30".
> gzip had "-9"
> Turtledeflate has 1MB superblock, 512 blocks max and "--9"

### Compression Ratio

| File               | Note     | gzip     | Zopfli   | Turtledeflate | Turtle / Zopfli |
|--------------------|----------|----------| ---------|---------------|-----------------|
| alice29.txt.gz     | Cantrbry | 54191    | 51473    | 51422         | 0.999009        |
| asyoulik.txt.gz    | Cantrbry | 48829    | 46345    | 46280         | 0.998597        |
| cp.html.gz         | Cantrbry | 7981     | 7714     | 7702          | 0.998444        |
| fields.c.gz        | Cantrbry | 3136     | 3018     | 3016          | 0.999337        |
| grammar.lsp.gz     | Cantrbry | 1246     | 1196     | 1194          | 0.998327        |
| kennedy.xls.gz     | Cantrbry | 209733   | 181412   | 175425        | 0.966997        |
| lcet10.txt.gz      | Cantrbry | 144429   | 137246   | 136950        | 0.997843        |
| plrabn12.txt.gz    | Cantrbry | 194277   | 183969   | 183682        | 0.998439        |
| ptt5.gz            | Cantrbry | 52382    | 48575    | 48506         | 0.998579        |
| sum.gz             | Cantrbry | 12772    | 11597    | 11563         | 0.997068        |
| xargs.1.gz         | Cantrbry | 1756     | 1706     | 1704          | 0.998827        |
| dickens.gz         | Silesia  | 3851823  | 3672829  | 3664991       | 0.997865        |
| mozilla.gz         | Silesia  | 18994142 | 18075778 | 18035457      | 0.997769        |
| mr.gz              | Silesia  | 3673940  | 3439166  | 3420236       | 0.994495        |
| nci.gz             | Silesia  | 2987533  | 2739962  | 2735758       | 0.998465        |
| ooffice.gz         | Silesia  | 3090442  | 2987867  | 2985017       | 0.999046        |
| osdb.gz            | Silesia  | 3716342  | 3607235  | 3605896       | 0.999628        |
| reymont.gz         | Silesia  | 1820834  | 1692000  | 1691218       | 0.999537        |
| samba.gz           | Silesia  | 5408272  | 5095092  | 5085326       | 0.998083        |
| sao.gz             | Silesia  | 5327041  | 5240993  | 5234480       | 0.998757        |
| webster.gz         | Silesia  | 12061624 | 11517716 | 11505585      | 0.998946        |
| x-ray.gz           | Silesia  | 6037713  | 5743093  | 5739938       | 0.999450        |
| xml.gz             | Silesia  | 662284   | 626427   | 625708        | 0.998852        |
| enwik8.gz          | enwik8   | 36445248 | 34960932 | 34932605      | 0.999189        |
| pak0.pak.gz        | Quake    | 8561915  | 7787285  | 7756348       | 0.996027        |



### Speed in Seconds

> Running Single Core on an AMD Ryzen Threadripper 3970X
> Number of Blocks Turtledeflate used is in parenthesis.

| File               | Note     | gzip | Zopfli | Turtledeflate |
|--------------------|----------|------|--------|---------------|
| alice29.txt.gz     | Cantrbry | 0    | 1      | 26 (3)        |
| asyoulik.txt.gz    | Cantrbry | 0    | 1      | 26 (3)        |
| cp.html.gz         | Cantrbry | 0    | 1      | 8 (1)         |
| fields.c.gz        | Cantrbry | 0    | 1      | 9 (2)         |
| grammar.lsp.gz     | Cantrbry | 0    | 1      | 3 (1)         |
| kennedy.xls.gz     | Cantrbry | 1    | 9      | 137 (12)      |
| lcet10.txt.gz      | Cantrbry | 0    | 2      | 97 (4)        |
| plrabn12.txt.gz    | Cantrbry | 0    | 7      | 61 (3)        |
| ptt5.gz            | Cantrbry | 0    | 6      | 343 (3)       |
| sum.gz             | Cantrbry | 0    | 1      | 66 (11)       |
| xargs.1.gz         | Cantrbry | 0    | 1      | 3 (1)         |
| dickens.gz         | Silesia  | 1    | 53     | 1463 (39)     |
| mozilla.gz         | Silesia  | 6    | 755    | 72218 (3477)* |
| mr.gz              | Silesia  | 1    | 95     | 11079 (254)   |
| nci.gz             | Silesia  | 2    | 719    | 13871 (170)   |
| ooffice.gz         | Silesia  | 1    | 29     | 4016 (134)    |
| osdb.gz            | Silesia  | 0    | 30     | 1163 (12)     |
| reymont.gz         | Silesia  | 1    | 35     | 956 (21)      |
| samba.gz           | Silesia  | 1    | 231    | 14312 (523)   |
| sao.gz             | Silesia  | 0    | 29     | 6770 (202)    |
| webster.gz         | Silesia  | 4    | 187    | 5628 (62)     |
| x-ray.gz           | Silesia  | 0    | 26     | 2251 (48)     |
| xml.gz             | Silesia  | 0    | 60     | 2506 (54)     |
| enwik8.gz          | enwik8   | 5    | 448    | 22758 (521)   |
| pak0.pak.gz        | Quake    | 3    | 614    | 78523 (4033)* |

> * lol..

Yes, Turtledeflate is absurdly slow.




## Notes

Source code quality could be better.

I stole a function ( OptimizeHuffmanForRle() ) from Zopfli.

Sadly it might be possible that in some cases Turtledeflate gets stuck in an endless loop.

Turtledeflate is really slow. Most time is spent in the block splitter. Threading could be done easily on superblocks. More parts of the block splitter maybe do not have to run in late block splitter states. If the block splitter state does a re-run at the same state level and a subblock did not have its boundaries changed it may be copied from the previous pass instead of being done again.

The amount of pressure 'mozilla' from Silesia and 'pak0.pak' from Id Softwares Quake put on Turtledeflates current blocksplitter implementation is brutal.

There is some logic in the blocksplitter that forces more block splits and checks the result if normal block splitting does not find a block split which decreases size. Most of the time this is wasted runtime but in some cases leads to more, better, blocks.

Tweaking things in turtledeflate_block_deflate_squish(), especially the randomizing compression path, most of the time makes things better and worse at the same time if the number of different paths tested is not increased. So if one wants to play around with that always check compression ratio on multiple different files.

Sometimes it is not important to produce something of value I guess.




