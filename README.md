# Arcusical
Windows 10 Music Player.

Arcusical is a simple music player for Windows 10. The primary motivation for developing this was to have a music player that intelligently handled duplicated music files including duplicated songs that were in different formats. This is something not many music players seem to do. Additionally much of my library is encoded as ALAC in an MPEG4 container. This is problematic as many players assume that .m4a is encoded in AAC and don't actually support ALAC.

I built this player so that I would have an app that made it really simple to listen to my music without having to spend time organizing my library or transcoding my songs. Also it was fun and I enjoy building stuff.

The player is still a work in progress 

## Build

Requires Visual Studio 2015.

To build Arcusical follow these steps:
```
git clone --recursive https://github.com/jesseDtucker/Arcusical.git
setup.bat
Arcusical.sln
```

Now just pick your target and build. Will work for x64 builds, will probably build for x86 but I removed support in the project config as it was causing unneeded overhead. Just be aware that the git clone will take a while because one of the submodules is [Boost](http://www.boost.org/). Also make sure you have a recent version of git. There is a bug in some of the older git versions that causes the submodules to not get pulled correctly.

After it has launched it will start automatically searching for music in your music folder. It does take a moment to get going so give it a bit of time. I'm working on improving the time-to-first-album but there is a fair bit of work required to do that. After the library is loaded startup will be fast as all metadata is cached.

Enjoy!

## FAQ

1. Why did you build this?

  Because I wanted to! That's really all there is to it. I couldn't find a music player I really liked and decided it would be fun to build my own. At the time I knew it would be a lot of work but I certainly underestimated the scope of what I assumed to be a 'simple' project.
  
2. What were the project goals?

  I wanted a player that was very simple, handled duplicate songs and unknown albums intelligently (ie. don't lump all unknown albums into a single massive album). It also had to be good at finding album art. Typically embedded album art is always used but when it's not available I want it to automatically search for something from disk. If that is not available use a default image that looks okay, ie. Not grey or faded. Finally playing whatever format of song that happens to be in my library is important. Windows did not support ALAC until Win 10 so most players that just rely on the built in codecs can't play this format without some effort by the user. I wanted something that would 'just work'.
  
3. Why a Windows 10 App?

  When I first envisioned this project Windows 8 was brand new. I wanted to try playing with some of the new tools that were available and I liked the new style of UI that Microsoft was pushing with Metro. So I built the app for Windows 8. However, I have taken some care to not make Arcusical completely dependent upon Windows and most of the code should work on other platforms. At some point I plan to build a second UI using QT so that I can run this on Ubuntu and maybe Android. Recently it was updated to be a Windows 10 Universal App.
  
4. Why C++?

  It was harder this way. I like the language as I feel it puts up more of a challenge but also gives me a fun puzzle to solve while writing my code. Realistically it probably would have been easier to build most of the App with C# and then just use C++ for the bits that needed it, like the Win8Player that uses COM to setup the ALAC decoder. I also really enjoy working with the language, and this is my side project so everything is about doing what is fun.
  
5. Is it done?

  I hope not! As it stands the player is still very simple. It can play music and has a simple search feature but that's really about it. The UI is pretty atrocious, there are none of the fancy features like playing from online radio or cloud storage that most players have, and it still doesn't support FLAC. These are all things I want to build out at some point. Additionally the audio deduplication is fairly rudimentary.
  
6. How does it detect duplicate songs?

  At present the detection is fairly simple but surprisingly effective. The metadata of songs are compared and those with the same title and artist are considered to be the same. This works in most cases but breaks if there are discrepancies between song files. What I want to add is some smarter detection at some point. The most effective approach would probably be to use a perceptual hash of each song and then compare. Any that are very close together would be the same song. Unfortunately this is non-trivial and will require a lot of CPU time to do. I will implement it someday but until then the meta data solution seems to be working fairly well. One other alternative would be to compare embedded album art. Although it could not tell the songs apart it could help with grouping duplicate albums together.
