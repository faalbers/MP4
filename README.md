# MP4 (QT) format Reader/Parser/Writer

[mp4 atom structures](https://developer.apple.com/library/archive/documentation/QuickTime/QTFF/QTFFChap2/qtff2.html#//apple_ref/doc/uid/TP40000939-CH204-56313)

[mp4 elements](https://www.cimarronsystems.com/wp-content/uploads/2017/04/Elements-of-the-H.264-VideoAAC-Audio-MP4-Movie-v2_0.pdf)

[timed metadata](https://developer.apple.com/library/archive/documentation/QuickTime/QTFF/QTFFChap3/qtff3.html#//apple_ref/doc/uid/TP40000939-CH205-SW130)

![mp4 atom size](https://i.stack.imgur.com/mN9Ph.jpg)
![mp4 atom structures](https://developer.apple.com/library/archive/documentation/QuickTime/QTFF/art/qtff_08.gif)

## TODO
* handle vmhd, smhd and gmhd atoms.\
gmhd/gpmd (gopro) just has an apparently 32 bit value that is not used yet but needs to be included.

* change atomnameTable to just table.

* transfer all structs to datatypes.hpp

