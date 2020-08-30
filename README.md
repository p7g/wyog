# wyog

I decided to follow along with the [Write yourself a Git!][1] guide, but on hard
mode. It seems like the easiest part will be learning how git works.

So far, this has been an exercise in:
- Working with the filesystem in C (\*nix only). I can't help but feel like I'm
  doing this wrong.
- INI parsing. The config-parsing code is not the robust-est.
- SHA1. When I said dependency-free, I meant it. I'm definitely not regretting
  that decision already...

There might still be memory/resource leaks that I missed (though there are few
allocations at this point); I have yet to run it with valgrind.

[1]: https://wyag.thb.lt/
