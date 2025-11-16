# AbiMusicSheet (AMS)

**AbiMusicSheet (AMS)** is a completely **nonsensical, quirky project** I made for fun. I’m a programmer who can read pseudocode effortlessly but, for the life of me, cannot read traditional piano sheet music (┐(´д｀)┌).

So I made **a human-readable coded music sheet language** that lets me write piano music like I write code. Segments, LEFT/RIGHT hands, chords, durations, and syncable chunks are all **designed for a programmer’s eyes**, not a musician’s.

ヽ(・∀・)ﾉ Fun, absurd, and totally impractical — exactly what bored programmers like me do.

---

## Features

* New: AMS to JSON Parser, use "g++ -std=c++11 -O2 -o ams_parser ams_parser.cpp" to compile ams_parser.cpp or download the Unix executable.

* **Segments:**
  Reusable blocks of music like CHORUS, VERSE, or tiny motifs.
  `(⌐■_■) Just call them whenever you want to repeat stuff.`

* **Hands separation:**
  `.LEFT` = sequential notes
  `.RIGHT` = chords
  `(≧▽≦) Your eyes can scan the music like code blocks.`

* **Readable syntax:**

  * `1,2,3` → sequential notes
  * `1.3.5` → chord (play simultaneously)
  * `#` / `b` → sharp / flat
  * `^n` / `^-n` → pitch shift relative to base
  * `'` = short hold, `"` = long hold

* **Chunk alignment:**
  `||` separates phrases so LEFT and RIGHT hands stay in sync.
  `(＾▽＾)ﾉ Perfect for scanning at a glance.`

* **SYNC():**
  Forces alignment between LEFT notes and RIGHT chords.
  `(￣▽￣)b No messy timing headaches.`

* **Loops & repeats:**
  `Repeat(3){ Segment(1, CHORUS); }`
  `(¬‿¬) Because programmers love reusability.`

---

## Why AMS exists

* Purely for **fun** ( ͡° ͜ʖ ͡°)
* For programmers who can read pseudocode but not sheet music
* To make writing piano music feel like coding instead of… music notation
* Because boredom is a powerful creative fuel
* And because… why not? (≧◡≦) ♡

---

## How to Use (Very Basic Overview)

```c
Tempo(100);

Segment(1, CHORUS) {
    Begin.LEFT{
        SYNC();
        1,2,1,2,3,4,6,4,3 || 3,6,3',5',5
    }
    Begin.RIGHT{
        SYNC();
        1.3.5 || 3.4.6 || 5.7.2
    }
    END;
}

Repeat(2){ Segment(1, CHORUS); }
```

* Read it like code, not sheet music (¬‿¬)
* LEFT = melody, RIGHT = chords
* `||` = chunk separation
* `'` and `"` = note duration

---

## Philosophy

AMS is **a love letter to programmer brains everywhere**:

* Music you can scan like code
* Patterns you can repeat like functions
* Completely absurd, slightly useless, and totally fun (｡♥‿♥｡)

---
## I'm dumb, made this absolute nonsense for fun T^T
