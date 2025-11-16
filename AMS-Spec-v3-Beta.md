# AbiMusicSheet (AMS) Language Specification v3.0-Beta

**A code-structured music notation system for programmers who can't read sheet music**

```
  ♪┌(・o・)┘♪┐ ( ・o・) ┌♪
  Made for quirky brains
```

**Document Version:** 3.0.0-Beta  
**Status:** Production-Ready Candidate  
**License:** MIT

---

## Table of Contents

### Part I: Core Concepts
1. [Philosophy & Design Goals](#philosophy)
2. [Quick Start Guide](#quickstart)
3. [File Structure](#file-structure)
4. [The Map Block (Key System)](#map-block)

### Part II: Note-Level Syntax
5. [Notes & Scale Degrees](#notes)
6. [Durations (Rhythm)](#durations)
7. [Accidentals (Sharp/Flat)](#accidentals)
8. [Octave Shifts](#octaves)
9. [Rests](#rests)

### Part III: Musical Expression
10. [Chords (Simultaneous Notes)](#chords)
11. [Articulation (Staccato, Legato)](#articulation)
12. [Dynamics (Volume)](#dynamics)
13. [Tempo Control](#tempo)
14. [Pedaling](#pedaling)

### Part IV: Structure & Organization
15. [Hands (LEFT & RIGHT)](#hands)
16. [Synchronization (SYNC & ||)](#synchronization)
17. [Segments (Musical Sections)](#segments)
18. [Macros (Define & Use)](#macros)
19. [Main Block (Playback Order)](#main)

### Part V: Advanced Techniques
20. [Common Patterns](#patterns)
21. [Hand Independence](#hand-independence)
22. [Advanced Rhythms](#advanced-rhythms)
23. [Ornaments](#ornaments)

### Part VI: Complete Examples
24. [Example: Twinkle Twinkle (Grade 0)](#example-twinkle)
25. [Example: Happy Birthday (Grade 1)](#example-birthday)
26. [Example: Minuet Pattern (Grade 2)](#example-minuet)

### Part VII: Reference
27. [Complete Syntax Reference](#syntax-reference)
28. [Error Guide](#error-guide)

---

## Part I: Core Concepts

## 1. Philosophy & Design Goals {#philosophy}

AMS is designed for **analytical minds** who think in code, not staves.

### Core Principles

**Visual Scanability**  
Read music like code blocks. Your eyes should jump between aligned chunks naturally.

**Consistent Hand Convention**  
- **LEFT hand** → Bass, accompaniment, chord progressions (lower notes)
- **RIGHT hand** → Melody, main tune (higher notes)

**Minimal Cognitive Load**  
Few symbols, predictable rules. No guessing.

**No Music Theory Required**  
Use integers (1-7) instead of note names. The `Map` block handles the theory.

**Function-Like Structure**  
Segments are reusable functions. Main() is your entry point.

### The Golden Rule

```
In AMS:
  LEFT  = What your left hand plays (usually chords/bass)
  RIGHT = What your right hand plays (usually melody)
  
This is consistent. Always. No exceptions.
```

---

## 2. Quick Start Guide {#quickstart}

### Your First AMS File

```ams
// Metadata
Title: "My First Song"
Key: C
Tempo: 120

// Map numbers to notes
Map { Key: C; Scale: Major; }
// Now: 1=C, 2=D, 3=E, 4=F, 5=G, 6=A, 7=B

// Define a segment (a reusable section)
Segment(1, INTRO) {
    // LEFT hand plays chords (notes together)
    Begin.LEFT {
        SYNC();
        1.3.5.h || 4.6.1.h || 5.7.2.h
        //  ^     chord held for 2 beats
    }
    
    // RIGHT hand plays melody (notes in sequence)
    Begin.RIGHT {
        SYNC();
        1, 2, 3, 4, 5.h || 5, 4, 3, 2, 1.h
        //  ^     notes played one after another
    }
    END;
}

// Define playback order
Main() {
    Segment(1, INTRO);
}
```

**Key Takeaways:**
- `,` = play notes **one after another** (melody)
- `.` = play notes **at the same time** (chord)
- `.h` = duration suffix (half note = 2 beats)
- `||` = phrase boundary (keeps hands aligned)

---

## 3. File Structure {#file-structure}

Every AMS file has four sections, in order:

```ams
// ============================================
// 1. METADATA (Optional but recommended)
// ============================================
Title: "Song Name"
Composer: "Your Name"
Key: C
Tempo: 120
TimeSignature: 4/4
Difficulty: 2

// ============================================
// 2. MAP BLOCK (Required)
// ============================================
Map { Key: C; Scale: Major; }

// ============================================
// 3. DEFINITIONS (Segments and Macros)
// ============================================
Define PATTERN_NAME { ... }
Segment(1, SECTION_NAME) { ... }

// ============================================
// 4. MAIN BLOCK (Required - playback order)
// ============================================
Main() {
    Segment(1, SECTION_NAME);
}
```

---

## 4. The Map Block {#map-block}

The Map block translates numbers (1-7) into actual notes based on a key and scale.

### Basic Usage

```ams
// C Major
Map { Key: C; Scale: Major; }
// Result: 1=C, 2=D, 3=E, 4=F, 5=G, 6=A, 7=B

// A Minor
Map { Key: A; Scale: Minor; }
// Result: 1=A, 2=B, 3=C, 4=D, 5=E, 6=F, 7=G

// G Major
Map { Key: G; Scale: Major; }
// Result: 1=G, 2=A, 3=B, 4=C, 5=D, 6=E, 7=F#
```

### Supported Scales

| Scale Type | Example | Character |
|------------|---------|-----------|
| Major | Bright, happy | C Major = C D E F G A B |
| Minor | Sad, dramatic | A Minor = A B C D E F G |
| HarmonicMinor | Spooky, exotic | C HarmonicMinor = C D Eb F G Ab B |

**Philosophy:** Set the Map once, then write `1, 2, 3` and it will always sound correct for that key.

---

## Part II: Note-Level Syntax

## 5. Notes & Scale Degrees {#notes}

Notes are written as **integers from 1 to 7**, representing scale degrees.

```ams
// In C Major (Map { Key: C; Scale: Major; })
1  // C (root note)
2  // D
3  // E
4  // F
5  // G
6  // A
7  // B
```

**Important:** The actual pitch depends on your Map block and which hand is playing.

---

## 6. Durations (Rhythm) {#durations}

Duration suffixes control how long a note is held.

| Suffix | Name | Beats (in 4/4) | Example |
|--------|------|----------------|---------|
| (none) | Quarter Note | 1 | `1` |
| `.h` | Half Note | 2 | `1.h` |
| `.w` | Whole Note | 4 | `1.w` |
| `.e` | Eighth Note | 0.5 | `1.e` |
| `.s` | Sixteenth Note | 0.25 | `1.s` |

### Dotted Notes

Add `.` after the duration suffix to multiply by 1.5:

```ams
1.h.   // Dotted half note = 3 beats (2 × 1.5)
1.     // Dotted quarter note = 1.5 beats (1 × 1.5)
1.e.   // Dotted eighth note = 0.75 beats (0.5 × 1.5)
```

**For chords:** The dot applies to the entire chord duration:

```ams
1.3.5.     // Dotted quarter chord = all notes held 1.5 beats
1.3.5.h.   // Dotted half chord = all notes held 3 beats
```

### Tied Notes

Use `_` to extend a note's duration across multiple notes:

```ams
1.h_1.h  // Hold note 1 for 4 beats total (2 + 2)
1_1_1    // Hold note 1 for 3 beats (1 + 1 + 1)
1.w_1.h  // Hold note 1 for 6 beats (4 + 2)
```

**Important:** Ties connect the *same note* played multiple times. The notation `1_2_3` is invalid—you cannot tie different notes together. Use ties only for extending a single note's duration.

### Examples

```ams
// Melody with varied rhythm
1, 2, 3.h, 4.e, 5.e, 6.w

// Chord held for entire measure
1.3.5.w

// Quick repeated notes
1.e, 1.e, 1.e, 1.e
```

---

## 7. Accidentals (Sharp/Flat) {#accidentals}

Override the Map with `#` (sharp) or `b` (flat):

```ams
1#   // C# (one semitone up from C)
3b   // Eb (one semitone down from E)
7#   // B# (enharmonic to C)
```

**When to use:**
- Chromatic passages
- Non-diatonic notes
- Jazz/blues inflections

```ams
// Blues scale feeling
1, 3b, 4, 4#, 5, 7b, 1
```

---

## 8. Octave Shifts {#octaves}

Use `^` to shift octaves relative to the hand's default position.

```ams
1      // Note 1 in default octave
1^1    // Note 1, one octave higher
1^2    // Note 1, two octaves higher
1^-1   // Note 1, one octave lower
```

### Default Octave Positions

```ams
Settings {
    Octave.LEFT(3);   // Left hand defaults to octave 3 (lower)
    Octave.RIGHT(4);  // Right hand defaults to octave 4 (middle C region)
}
```

**Per-Segment Position Override:**

Use `Position()` to set the default octave for a hand within a segment:

```ams
Begin.LEFT {
    Position(C3);  // Set left hand to C3 (octave 3, scientific pitch notation)
    // Or simply:
    Position(3);   // Set left hand to octave 3
}
```

**Scientific Pitch Notation:**
- `Position(C3)` → Sets octave to 3 (C3 is the C below middle C)
- `Position(4)` → Sets octave to 4 (middle C region)
- `Position(C#3)` → Sharp/flat notes are supported
- `Position(5)` → Sets octave to 5 (higher register)

**Note:** If using the note-name format (e.g., `C3`), the octave number follows scientific pitch notation where C4 is middle C.

### Example: Spanning Multiple Octaves

```ams
Begin.RIGHT {
    // Ascending two octaves
    1, 2, 3, 4, 5, 6, 7, 1^1, 2^1, 3^1
    
    // Descending back down
    3^1, 2^1, 1^1, 7, 6, 5, 4, 3, 2, 1
}
```

---

## 9. Rests {#rests}

Rests create silence. They use the same duration suffixes as notes.

```ams
R      // Quarter rest (1 beat)
R.h    // Half rest (2 beats)
R.w    // Whole rest (4 beats)
R.e    // Eighth rest (0.5 beats)
R.s    // Sixteenth rest (0.25 beats)
```

### Examples

```ams
// Rest between phrases
1, 2, 3, R, 5, 6, 7

// Syncopated rhythm
1, R.e, 2.e, R.e, 3.e, R.e, 4.e
```

---

## Part III: Musical Expression

## 10. Chords (Simultaneous Notes) {#chords}

The `.` operator plays notes **at the same time**.

### Basic Chords

```ams
1.3.5     // C major triad (root position)
4.6.1     // F major triad
5.7.2     // G major triad
```

### Chords with Duration

```ams
1.3.5.h   // Hold chord for 2 beats
1.3.5.w   // Hold chord for 4 beats
1.3.5.e   // Quick chord (0.5 beats)
```

### Chord Inversions

Use octave shifts to create inversions:

```ams
// C Major in different voicings
1.3.5        // Root position (C-E-G)
3.5.1^1      // First inversion (E-G-C)
5.1^1.3^1    // Second inversion (G-C-E)
```

### Left Hand Chord Patterns

```ams
Begin.LEFT {
    SYNC();
    // Typical chord progression
    1.3.5.h || 4.6.1.h || 5.7.2.h || 1.3.5.w
}
```

---

## 11. Articulation {#articulation}

Articulation marks change **how** notes are played.

| Symbol | Name | Meaning | Example |
|--------|------|---------|---------|
| `!` | Staccato | Short, detached | `1!, 2!, 3!` |
| `~` | Legato | Smooth, connected | `1~, 2~, 3` |
| `>` | Accent | Emphasize | `1, 2>, 3` |
| `(h)` | Fermata | Hold longer | `1.w(h)` |

### Examples

```ams
// Bouncy staccato melody
Begin.RIGHT {
    1!, 2!, 3!, 4!, 5!
}

// Smooth legato phrase
Begin.RIGHT {
    1~, 2~, 3~, 4~, 5
}

// Mixed articulation
Begin.RIGHT {
    1!, 2~, 3>, 4!, 5~
}
```

---

## 12. Dynamics (Volume) {#dynamics}

Dynamic markings control volume.

| Symbol | Name | Loudness |
|--------|------|----------|
| `pp` | Pianissimo | Very soft |
| `p` | Piano | Soft |
| `mp` | Mezzo-piano | Moderately soft |
| `mf` | Mezzo-forte | Moderately loud (default) |
| `f` | Forte | Loud |
| `ff` | Fortissimo | Very loud |

### As Note Suffixes

```ams
Begin.RIGHT {
    1p, 2mp, 3mf, 4f, 5ff
}
```

### As Blocks

```ams
Begin.RIGHT {
    Dynamic(p) {
        1, 2, 3, 4, 5
    }
}
```

### Crescendo & Decrescendo

```ams
// Gradually get louder
<(1p, 2mp, 3mf, 4f, 5ff)

// Gradually get softer
>(5ff, 4f, 3mf, 2mp, 1p)
```

---

## 13. Tempo Control {#tempo}

### Setting Tempo

```ams
// Global default
Tempo: 120  // In metadata

// Per-segment override
Segment(1, INTRO) {
    Tempo(140);
    // ... music ...
}
```

### Gradual Changes

```ams
// Slow down over time
Ritardando(80) {
    1, 2, 3, 4, 5
}

// Speed up over time
Accelerando(160) {
    1, 2, 3, 4, 5
}
```

---

## 14. Pedaling {#pedaling}

Control the sustain pedal (damper pedal):

```ams
Pedal.DOWN;   // Press pedal
Pedal.UP;     // Release pedal
Pedal.CHANGE; // Quick lift-and-press (clear old, catch new)
```

### Common Pattern

```ams
Begin.LEFT {
    Pedal.DOWN;
    1.3.5.w
    Pedal.CHANGE;
    4.6.1.w
    Pedal.UP;
}
```

---

## Part IV: Structure & Organization

## 15. Hands (LEFT & RIGHT) {#hands}

Every segment has two hand blocks:

```ams
Segment(1, EXAMPLE) {
    Begin.LEFT {
        Position(C3);  // Set default octave
        // Left hand music (usually chords/bass)
    }
    
    Begin.RIGHT {
        Position(C4);  // Middle C region
        // Right hand music (usually melody)
    }
    END;
}
```

### The Convention

```
LEFT  = Bass, accompaniment, chord roots
RIGHT = Melody, main tune, countermelody
```

This is **consistent throughout AMS**. No confusion.

---

## 16. Synchronization {#synchronization}

### SYNC() - Explicit Alignment

Place `SYNC();` at the start of each hand block to ensure they begin together:

```ams
Begin.LEFT {
    SYNC();
    1.3.5.w
}

Begin.RIGHT {
    SYNC();
    1, 2, 3, 4, 5, 6, 7, 1^1
}
```

**What happens without SYNC():**

If you omit `SYNC()`, the hands will start playing independently as soon as their code is reached. This can cause timing drift and is **not recommended** for most music. Always use `SYNC()` at the beginning of hand blocks unless you specifically need asynchronous playback (advanced technique).

### || - Phrase Boundaries

The `||` operator creates **synchronization points** between hands:

```ams
Begin.LEFT {
    SYNC();
    1.3.5.h || 4.6.1.h || 5.7.2.h
}

Begin.RIGHT {
    SYNC();
    1, 2, 3, 4 || 5, 6, 7, 1^1 || 1^1, 7, 6, 5
}
// Each chunk must have equal total duration
```

### Visual Alignment

```ams
// Well-aligned chunks
Begin.LEFT {
    SYNC();
    1.3.5.w      || 4.6.1.w      || 5.7.2.w
}

Begin.RIGHT {
    SYNC();
    1,2,3,4,5,6,7,1^1 || 2^1,1^1,7,6,5,4,3,2 || 1,2,3,4,5.w
}
```

**Rule:** Total beats in LEFT between `||` must equal total beats in RIGHT.

---

## 17. Segments {#segments}

Segments are reusable musical sections (like functions).

### Syntax

```ams
Segment(<id>, <NAME>) {
    Tempo(<bpm>);  // Optional tempo override
    
    Begin.LEFT {
        Position(<Note>);
        SYNC();
        // Left hand music
    }
    
    Begin.RIGHT {
        Position(<Note>);
        SYNC();
        // Right hand music
    }
    END;
}
```

### Example

```ams
Segment(1, VERSE) {
    Begin.LEFT {
        SYNC();
        1.3.5.h || 4.6.1.h
    }
    
    Begin.RIGHT {
        SYNC();
        1, 2, 3, 4 || 5, 6, 7, 1^1
    }
    END;
}

Segment(2, CHORUS) {
    Begin.LEFT {
        SYNC();
        5.7.2.h || 1.3.5.w
    }
    
    Begin.RIGHT {
        SYNC();
        5, 6, 7, 1^1 || 1^1.w
    }
    END;
}
```

---

## 18. Macros {#macros}

Macros define reusable musical patterns.

### Define Syntax

```ams
Define <NAME> {
    // Musical pattern
}
```

### Use Syntax

```ams
Use(<NAME>);
```

### Examples

```ams
// Define common chord progression
Define PROGRESSION_I_IV_V {
    1.3.5.h || 4.6.1.h || 5.7.2.h
}

// Define a melodic riff
Define MELODY_MOTIF {
    1.e, 2.e, 3.e, 4.e, 5.h
}

// Define waltz bass pattern
Define WALTZ_BASS {
    1, 1.3.5, 1.3.5 || 4, 4.6.1, 4.6.1
}

// Use in a segment
Segment(1, VERSE) {
    Begin.LEFT {
        Use(WALTZ_BASS);
    }
    
    Begin.RIGHT {
        Use(MELODY_MOTIF);
    }
    END;
}
```

---

## 19. Main Block {#main}

The Main() block defines **playback order** (the entry point).

### Basic Usage

```ams
Main() {
    Segment(1, INTRO);
    Segment(2, VERSE);
    Segment(3, CHORUS);
    Segment(4, OUTRO);
}
```

### Repeat Loops

```ams
Main() {
    Segment(1, INTRO);
    
    Repeat(2) {
        Segment(2, VERSE);
        Segment(3, CHORUS);
    }
    
    Segment(4, OUTRO);
}

// Plays: INTRO, VERSE, CHORUS, VERSE, CHORUS, OUTRO
```

### Nested Loops

```ams
Main() {
    Repeat(3) {
        Segment(1, VERSE);
        
        Repeat(2) {
            Segment(2, CHORUS);
        }
    }
}
```

### Inline Notes

```ams
Main() {
    Segment(1, INTRO);
    
    // Quick transition
    LEFT: 1, 2, 3;
    RIGHT: 1.3.5;
    
    Segment(2, VERSE);
}
```

---

## Part V: Advanced Techniques

## 20. Common Patterns {#patterns}

### Alberti Bass (Broken Chord)

```ams
// Pattern: Root - Fifth - Third - Fifth
Define ALBERTI_C {
    1.e, 5.e, 3.e, 5.e, 1.e, 5.e, 3.e, 5.e
}

Define ALBERTI_F {
    4.e, 1^1.e, 6.e, 1^1.e, 4.e, 1^1.e, 6.e, 1^1.e
}
```

### Waltz Bass (Oom-Pah-Pah)

```ams
TimeSignature: 3/4

Define WALTZ_C {
    1, 1.3.5, 1.3.5
}

Define WALTZ_G {
    5, 5.7.2, 5.7.2
}
```

### Arpeggio Patterns

```ams
// Ascending arpeggio
Define ARP_UP {
    1.e, 3.e, 5.e, 1^1.e
}

// Descending arpeggio
Define ARP_DOWN {
    1^1.e, 5.e, 3.e, 1.e
}
```

---

## 21. Hand Independence {#hand-independence}

Different articulations in each hand:

```ams
Segment(1, INDEPENDENCE) {
    Begin.LEFT {
        SYNC();
        // Smooth, connected chords
        1.3.5.h~ || 4.6.1.h~ || 5.7.2.h~
    }
    
    Begin.RIGHT {
        SYNC();
        // Short, bouncy melody
        1.e!, 2.e!, 3.e!, 4.e!, 5.e!, 6.e!, 7.e!, 1^1.e! ||
        1^1.e!, 7.e!, 6.e!, 5.e!, 4.e!, 3.e!, 2.e!, 1.e! ||
        5.h
    }
    END;
}
```

---

## 22. Advanced Rhythms {#advanced-rhythms}

### Triplets

Fit 3 notes into the space of 2:

```ams
Triplet { 1.e, 2.e, 3.e }  // 3 eighth notes in 1 beat
```

### Dotted Rhythms

```ams
// Long-short pattern
1., 2.e, 3., 4.e
```

---

## 23. Ornaments {#ornaments}

### Grace Notes

Quick, unmetered notes before the main note:

```ams
Grace(7) { 1^1.h }  // Quick 7 before holding 1^1
```

### Trills

Rapid alternation between two notes:

```ams
Trill(1, 2) { 1.h }  // Trill between 1 and 2 for duration of half note
```

---

## Part VI: Complete Examples

## 24. Example: Twinkle Twinkle Little Star {#example-twinkle}

```ams
Title: "Twinkle Twinkle Little Star"
Key: C
Tempo: 120
TimeSignature: 4/4
Difficulty: 0

Map { Key: C; Scale: Major; }

Settings {
    Octave.LEFT(3);
    Octave.RIGHT(4);
}

Segment(1, VERSE) {
    Begin.LEFT {
        SYNC();
        1.3.5.h, 1.3.5.h || 4.6.1.h, 2.4.6.h
    }
    
    Begin.RIGHT {
        SYNC();
        1, 1, 5, 5, 6, 6, 5.h || 4, 4, 3, 3, 2, 2, 1.h
    }
    END;
}

Main() {
    Repeat(2) {
        Segment(1, VERSE);
    }
}
```

---

## 25. Example: Happy Birthday {#example-birthday}

```ams
Title: "Happy Birthday"
Key: C
Tempo: 120
TimeSignature: 3/4
Difficulty: 1

Map { Key: C; Scale: Major; }

Define WALTZ_C { 1, 1.3.5, 1.3.5 }
Define WALTZ_G { 5, 5.7.2, 5.7.2 }
Define WALTZ_F { 4, 4.6.1, 4.6.1 }

Segment(1, LINE1) {
    Begin.LEFT {
        SYNC();
        Use(WALTZ_C) || Use(WALTZ_G)
    }
    
    Begin.RIGHT {
        SYNC();
        R.e, 5.e, 5., 6, 5, 1^1.h || 7.h.
    }
    END;
}

Segment(2, LINE2) {
    Begin.LEFT {
        SYNC();
        Use(WALTZ_C) || Use(WALTZ_F)
    }
    
    Begin.RIGHT {
        SYNC();
        R.e, 5.e, 5., 6, 5, 2^1.h || 1^1.h.
    }
    END;
}

Segment(3, LINE3) {
    Begin.LEFT {
        SYNC();
        Use(WALTZ_C) || Use(WALTZ_F)
    }
    
    Begin.RIGHT {
        SYNC();
        R.e, 5.e, 5., 5^1, 3^1.h, 1^1.h || 7., 6.h
    }
    END;
}

Segment(4, LINE4) {
    Tempo(100);
    
    Begin.LEFT {
        SYNC();
        Use(WALTZ_G) || Use(WALTZ_C)
    }
    
    Begin.RIGHT {
        SYNC();
        R.e, 4^1.e, 4^1., 3^1, 1^1.h || 2^1.h, 1^1.h(h)
    }
    END;
}

Main() {
    Segment(1, LINE1);
    Segment(2, LINE2);
    Segment(3, LINE3);
    Segment(4, LINE4);
}
```

---

## 26. Example: Minuet Pattern {#example-minuet}

```ams
Title: "Simple Minuet"
Key: G
Tempo: 110
TimeSignature: 3/4
Difficulty: 2

Map { Key: G; Scale: Major; }

Define PATTERN_A {
    5.e, 1.e, 2.e, 3.e, 4.e, 5.e
}

Define PATTERN_B {
    1.e, 3.e, 5.e, 1^1.e, 7.e, 6.e
}

Segment(1, PART_A) {
    Begin.LEFT {
        Position(G2);
        SYNC();
        1, 1.3.5, 1.3.5 || Use(PATTERN_A)
    }
    
    Begin.RIGHT {
        Position(G4);
        SYNC();
        5.hp, 4 || 3, 2, 1
    }
    END;
}

Segment(2, PART_B) {
    Begin.LEFT {
        Position(G2);
        SYNC();
        4, 4.6.1, 4.6.1 || Use(PATTERN_B)
    }
    
    Begin.RIGHT {
        Position(G4);
        SYNC();
        2.hf, 3 || 4, 3, 2
    }
    END;
}

Main() {
    Segment(1, PART_A);
    Segment(2, PART_B);
    Segment(1, PART_A);
}
```

---

## Part VII: Reference

## 27. Complete Syntax Reference {#syntax-reference}

### File Structure Keywords

| Keyword | Usage | Example |
|---------|-------|---------|
| Title | Song name | `Title: "My Song"` |
| Composer | Author | `Composer: "A. Programmer"` |
| Key | Default key | `Key: C` |
| Tempo | Default BPM | `Tempo: 120` |
| TimeSignature | Meter | `TimeSignature: 4/4` |
| Difficulty | Grade level (0-5) | `Difficulty: 2` |

### Map Block

```ams
Map {
    Key: <Note>;
    Scale: <Major|Minor|HarmonicMinor>;
}
```

### Notes & Modifiers

| Symbol | Meaning | Example |
|--------|---------|---------|
| 1-7 | Scale degree | `1, 2, 3` |
| # | Sharp | `1#` |
| b | Flat | `3b` |
| ^n | Octave up n | `1^1, 5^2` |
| ^-n | Octave down n | `1^-1` |

### Durations

| Suffix | Duration | Beats (4/4) |
|--------|----------|-------------|
| (none) | Quarter | 1 |
| .h | Half | 2 |
| .w | Whole | 4 |
| .e | Eighth | 0.5 |
| .s | Sixteenth | 0.25 |
| . | Dot (add 50%) | `1. = 1.5` |
| _ | Tie | `1_2` |

### Rests

| Symbol | Duration | Beats (4/4) |
|--------|----------|-------------|
| R | Quarter rest | 1 |
| R.h | Half rest | 2 |
| R.w | Whole rest | 4 |
| R.e | Eighth rest | 0.5 |
| R.s | Sixteenth rest | 0.25 |

### Separators

| Symbol | Meaning | Usage |
|--------|---------|-------|
| , | Sequential | `1, 2, 3` (melody) |
| . | Simultaneous | `1.3.5` (chord) |
| \|\| | Phrase boundary | Sync point between hands |
| ; | Statement end | In Main() block |
| // | Comment | `// This is a comment` |

### Articulation

| Symbol | Name | Effect |
|--------|------|--------|
| ! | Staccato | Short, detached |
| ~ | Legato | Smooth, connected |
| > | Accent | Emphasize |
| (h) | Fermata | Hold longer |

### Dynamics

| Symbol | Name | Volume |
|--------|------|--------|
| pp | Pianissimo | Very soft |
| p | Piano | Soft |
| mp | Mezzo-piano | Moderately soft |
| mf | Mezzo-forte | Moderately loud |
| f | Forte | Loud |
| ff | Fortissimo | Very loud |

### Tempo

```ams
Tempo(<bpm>);                    // Set tempo
Ritardando(<target_bpm>) { }     // Slow down gradually
Accelerando(<target_bpm>) { }    // Speed up gradually
```

### Pedaling

```ams
Pedal.DOWN;     // Press sustain pedal
Pedal.UP;       // Release pedal
Pedal.CHANGE;   // Quick lift-and-press
```

### Hand Blocks

```ams
Begin.LEFT {
    Position(<Note>);  // Set default octave
    SYNC();            // Align with other hand
    // Music here
}

Begin.RIGHT {
    Position(<Note>);
    SYNC();
    // Music here
}
END;
```

### Segments

```ams
Segment(<id>, <NAME>) {
    Tempo(<bpm>);  // Optional
    
    Begin.LEFT { ... }
    Begin.RIGHT { ... }
    END;
}
```

### Macros

```ams
Define <NAME> {
    // Musical pattern
}

// Use it later
Use(<NAME>);
```

### Main Block

```ams
Main() {
    Segment(<id>, <NAME>);
    
    Repeat(<count>) {
        Segment(<id>, <NAME>);
    }
}
```

### Advanced Features

```ams
// Crescendo/Decrescendo
<(1p, 2mp, 3f)     // Get louder
>(5f, 4mp, 3p)     // Get softer

// Dynamic blocks
Dynamic(<level>) {
    // All notes inherit dynamic
}

// Grace notes
Grace(<note>) { <main_note> }

// Triplets
Triplet { <note1>, <note2>, <note3> }

// Trills
Trill(<note1>, <note2>) { <duration> }
```

---

## 28. Error Guide {#error-guide}

### Common Errors & Solutions

#### Error: "Mismatched durations between LEFT and RIGHT"

**Problem:** Total beats don't match between hands in a chunk.

```ams
// WRONG
Begin.LEFT {
    SYNC();
    1.3.5.w  // 4 beats
}
Begin.RIGHT {
    SYNC();
    1, 2, 3  // Only 3 beats
}
```

**Solution:** Make durations equal.

```ams
// CORRECT
Begin.LEFT {
    SYNC();
    1.3.5.w  // 4 beats
}
Begin.RIGHT {
    SYNC();
    1, 2, 3, R  // 4 beats (3 notes + 1 rest)
}
```

#### Error: "Undefined segment"

**Problem:** Calling a segment that doesn't exist.

```ams
Main() {
    Segment(1, INTRO);
    Segment(2, VERSE);  // Never defined!
}
```

**Solution:** Define all segments before Main().

```ams
Segment(2, VERSE) {
    // ...
}

Main() {
    Segment(1, INTRO);
    Segment(2, VERSE);  // Now it exists
}
```

#### Error: "Undefined macro"

**Problem:** Using a macro before defining it.

```ams
Segment(1, VERSE) {
    Begin.LEFT {
        Use(WALTZ_BASS);  // Not defined yet!
    }
}
```

**Solution:** Define macros before segments.

```ams
Define WALTZ_BASS {
    1, 1.3.5, 1.3.5
}

Segment(1, VERSE) {
    Begin.LEFT {
        Use(WALTZ_BASS);  // Works now
    }
}
```

#### Error: "Missing Map block"

**Problem:** No Map block in file.

**Solution:** Always include a Map block.

```ams
Map { Key: C; Scale: Major; }
```

#### Error: "Invalid tie notation"

**Problem:** Trying to tie different notes together.

```ams
// WRONG
1_2_3  // Cannot tie different notes
```

**Solution:** Ties only connect the same note. Use ties to extend duration:

```ams
// CORRECT
1_1_1      // Hold note 1 for 3 beats
1.h_1.h    // Hold note 1 for 4 beats
```

#### Error: "Missing Main block"

**Problem:** No Main() block to define playback.

**Solution:** Always include Main().

```ams
Main() {
    Segment(1, INTRO);
}
```

---

## Key Design Decisions (v3.0)

### 1. Hand Convention (FIXED)

**Problem in v1-v2:** Inconsistent documentation about which hand does what.

**v3.0 Solution:**
- LEFT = Bass, accompaniment, chords (lower notes)
- RIGHT = Melody, main tune (higher notes)
- This is now **absolute and consistent** throughout the spec.

### 2. Chord vs Melody Syntax (FIXED)

**Problem in v1-v2:** Confusion about when to use `.` vs `,`.

**v3.0 Clarification:**
- `.` = Simultaneous notes (CHORDS) → `1.3.5`
- `,` = Sequential notes (MELODY) → `1, 2, 3, 4, 5`
- This maps to the natural use case:
  - LEFT hand often plays chords: `1.3.5.h`
  - RIGHT hand often plays melody: `1, 2, 3, 4, 5`

### 3. Duration System (SIMPLIFIED)

**Problem in v1:** Used quotes (`'`, `"`) which was ambiguous.

**v3.0 Solution:**
- Clear suffixes: `.e` `.s` `.h` `.w`
- Dotted notes: Add `.` after duration
- No more counting quotes

### 4. Synchronization (CLARIFIED)

**Problem in v1-v2:** `SYNC()` was manual and error-prone.

**v3.0 Solution:**
- `SYNC()` is still explicit but now clearly documented
- `||` creates automatic sync points between hands
- Rule: Beats between `||` must match in both hands

### 5. Macro System (ENHANCED)

**v3.0 Additions:**
- `Define` for patterns
- `Use()` to invoke them
- Common patterns documented (Alberti, Waltz, Arpeggio)

---

## Design Philosophy Summary

```
(˘▾˘)~♪♪

AMS is built on these principles:

1. CONSISTENCY
   LEFT = chords/bass, RIGHT = melody
   Always. No exceptions.

2. SCANABILITY
   Aligned chunks with || markers
   Read it like code blocks

3. MINIMAL SYMBOLS
   Few operators, each with one clear meaning
   . = together, , = sequential

4. REUSABILITY
   Segments = functions
   Macros = constants
   Main() = entry point

5. NO THEORY REQUIRED
   Map block handles all the music theory
   You just write 1, 2, 3

6. EXPLICIT OVER IMPLICIT
   SYNC() tells you where alignment happens
   Durations are always written (.h, .e)
   No guessing
```

---

## Production Readiness Checklist

This v3.0-Beta is ready for:

✓ **Parser Implementation**
- Clear lexical tokens
- Unambiguous grammar
- Well-defined error cases

✓ **User Documentation**
- Complete examples from Grade 0-2
- Common patterns documented
- Error guide included

✓ **Pedagogical Use**
- Progressive difficulty (0-2+)
- Real songs as examples
- Clear learning path

✓ **Community Adoption**
- Consistent conventions
- Extensible (macros, segments)
- Plain text (git-friendly)

---

## Next Steps for Implementation

### Parser Requirements

1. **Lexer:** Tokenize notes, separators, keywords
2. **Map Resolver:** Convert 1-7 to actual notes based on key/scale
3. **Duration Calculator:** Verify chunk alignment between hands
4. **Segment Registry:** Track defined segments and macros
5. **Execution Engine:** Process Main() block in order

### Validation Rules

1. Every file must have Map and Main blocks
2. Segments must be defined before use
3. Macros must be defined before use
4. Duration totals must match between LEFT/RIGHT within each `||` chunk
5. Note numbers must be 1-7 (or with accidentals)
6. Octave shifts must be integers

### Output Targets

- **MIDI:** Direct conversion for playback
- **MusicXML:** For importing to notation software
- **Sheet Music PDF:** Visual representation
- **Audio:** Synthesized piano sound

---

## Closing Thoughts

```
(ﾉ◕ヮ◕)ﾉ*:･ﾟ✧

You created AMS because traditional sheet music didn't work for your brain.

v3.0 is mature, consistent, and production-ready.

If you can read code, you can read this.
If you can write functions, you can write music.

Your quirky brain is exactly what this language was built for.

Now go make some music.
```

---

**Version:** 3.0.0-Beta  
**Status:** Production-Ready Candidate  
**Last Updated:** 2025  
**License:** MIT  
**Maintained by:** The Quirky Brain Collective

---

## Quick Reference Card

```
BASIC SYNTAX
  1,2,3           Sequential notes (melody)
  1.3.5           Chord (simultaneous)
  ||              Phrase break (sync point)
  1.h             Half note (2 beats)
  1.e             Eighth note (0.5 beats)
  R               Rest
  1#, 3b          Sharp/Flat
  1^1             Octave up
  1p, 1f          Soft/Loud

STRUCTURE
  Map { Key: C; Scale: Major; }
  Segment(n, NAME) { }
  Main() { }
  Repeat(n) { }

HANDS
  Begin.LEFT { }     Chords/bass
  Begin.RIGHT { }    Melody
  SYNC();            Align hands
  Position(C4);      Set octave

MACROS
  Define NAME { }
  Use(NAME);

REMEMBER
  LEFT  = chords/bass (lower)
  RIGHT = melody (higher)
  .     = together
  ,     = sequential
```
