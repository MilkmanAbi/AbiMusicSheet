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

## Playing MIDI Files.

Use either a MIDI software of your choice or to play MIDI files with VLC, load a SoundFont (SF2). VLC uses FluidSynth, so it will not play MIDI unless a SoundFont is selected.

## 1. Open VLC

Open the VLC media player.

## 2. Go to Preferences

Tools → Preferences
At the bottom-left, set:
Show Settings → All

## 3. Configure FluidSynth

Navigate to:
Input / Codecs → Audio codecs → FluidSynth

Find the field named:
SoundFont

Click Browse and select your `.sf2` file.

## 4. Save and restart VLC

VLC must be restarted for the settings to take effect.

---

## Philosophy

AMS is **a love letter to programmer brains everywhere**:

* Music you can scan like code
* Patterns you can repeat like functions
* Completely absurd, slightly useless, and totally fun (｡♥‿♥｡)

---

## AI Usage Disclosure

The **AMS spec and concept were entirely created by me** — every idea, quirk, odd design decision, and cursed spark of inspiration (￣▽￣)ノ

All documentation was **hand-typed**, rewritten several times, and shaped over many days. I also worked with my sister (who actually understands music lol) because I funnily enough **failed Grade 2 music**, uh... got 39/100 for basic Piano Skills, so… yeah, backup was needed (｡•́︿•̀｡)

AI was used **only** for two things:

* helping design **error-checking** for the parsers
* helping with **AMS → MIDI** implementation, because MIDI still looks like arcane wizardry to me, even after multiple YouTube deep dives, I seriously don't understand MIDI, I apologise.

Every concept, idea, structure, and weird lil eccentricity in this project is **human-made**.
It’s just a fun, personal project — nothing serious, no big agenda, just vibes and curiosity (๑˃ᴗ˂)ﻭ

---

## I'm dumb, made this absolute nonsense for fun T^T
