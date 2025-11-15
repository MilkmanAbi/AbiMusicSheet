# Abi Music Sheet (AMS) Language Specification v1.1

*A human-readable, code-structured music notation system for piano*

```
  ♪┏(・o･)┛♪┗ ( ･o･) ┓♪
  Made for quirky brains
```

---

## Table of Contents

1. [Philosophy & Design Goals](#philosophy)
2. [File Structure](#file-structure)
3. [Note Mapping System](#note-mapping)
4. [Core Syntax Elements](#core-syntax)
5. [Segment Structure](#segments)
6. [Control Flow](#control-flow)
7. [Macros & Reusability](#macros)
8. [Dynamics & Advanced Features](#advanced-features)
9. [Complete Examples](#examples)
10. [Syntax Reference Table](#syntax-reference)

---

## 1. Philosophy & Design Goals {#philosophy}

AMS is designed for **programmers, engineers, and analytical minds** who think in syntax, not staves.

### Core Principles:

- **Visual scanability** → Eyes jump between aligned chunks like reading code
- **Minimal cognitive load** → Few symbols, consistent rules  
- **Pattern recognition** → Repetitive structures are obvious at a glance
- **"No Music Theory" friendly** → Integer-based, relative pitch system
- **Machine-parsable but human-first** → Clean syntax that compiles and reads beautifully
- **Function-like invocation** → Call segments like functions, compose music programmatically

```
(￣▽￣)ノ
No sheet music? No problem!
```

---

## 2. File Structure {#file-structure}

Every AMS file follows this structure:

```ams
// ============================================
// METADATA BLOCK (Optional but recommended)
// ============================================
Title: "Song Name"
Artist: "Artist Name"
Key: C
DefaultTempo: 120
TimeSignature: 4/4

// ============================================
// NOTE MAPPING (Maps scale degrees to notes)
// ============================================
Map {
    Key: C;
    Scale: Major;
    // Auto-generates: 1=C, 2=D, 3=E, 4=F, 5=G, 6=A, 7=B
}

// ============================================
// GLOBAL SETTINGS
// ============================================
Settings {
    Tempo(120);
    Octave.LEFT(3);    // Left hand default octave
    Octave.RIGHT(4);   // Right hand default octave
}

// ============================================
// MACROS / DEFINITIONS (Reusable patterns)
// ============================================
Define WALTZ_BASS {
    1, 5, 5 || 4, 1, 1 || 5, 2, 2 || 1, 5, 5
}

Define CHORD_I {
    1.3.5
}

// ============================================
// SEGMENT DEFINITIONS (The actual music)
// ============================================
Segment(1, INTRO) {
    // ... music definition ...
}

Segment(2, VERSE) {
    // ... music definition ...
}

// ============================================
// MAIN EXECUTION (Playback order)
// ============================================
Main() {
    Segment(1, INTRO);
    Repeat(2) {
        Segment(2, VERSE);
    }
}
```

---

## 3. Note Mapping System {#note-mapping}

### 3.1 Integer-to-Note Mapping

Notes are mapped to integers based on scale degrees. The `Map` block defines this relationship.

#### Standard Major Scale (C Major):
```ams
Map {
    Key: C;
    Scale: Major;
}
// Result: 1=C, 2=D, 3=E, 4=F, 5=G, 6=A, 7=B
```

#### Standard Minor Scale (A Minor):
```ams
Map {
    Key: A;
    Scale: Minor;
}
// Result: 1=A, 2=B, 3=C, 4=D, 5=E, 6=F, 7=G
```

### 3.2 Octave System

By default:
- **LEFT hand** → Octave 3 (lower)
- **RIGHT hand** → Octave 4 (middle)

Override per segment or globally:

```ams
Settings {
    Octave.LEFT(2);   // Bass register
    Octave.RIGHT(5);  // High register
}
```

---

## 4. Core Syntax Elements {#core-syntax}

### 4.1 Note Modifiers

| Modifier | Meaning | Example | Result |
|----------|---------|---------|--------|
| `#` | Sharp (semitone up) | `1#` | C# |
| `b` | Flat (semitone down) | `3b` | Eb |
| `^n` | Octave shift up n octaves | `1^1` | C one octave up |
| `v_n` | Octave shift down n octaves | `5v_1` | G one octave down |
| `(h)` | Fermata (hold) | `1(h)` | Hold note longer |

### 4.2 Duration Markers

AMS uses **character-based suffixes** for duration, minimizing cognitive load (no counting quotes).

| Suffix | Meaning | Beats (in 4/4) |
|--------|---------|----------------|
| (none) | Quarter Note (default) | 1 beat |
| `.e` | Eighth Note | 0.5 beats |
| `.s` | Sixteenth Note | 0.25 beats |
| `.h` | Half Note | 2 beats |
| `.w` | Whole Note | 4 beats |
| `.` | Dotted Note (1.5x) | (e.g., `1.` = 1.5 beats, `1.h.` = 3 beats) |
| `_` | Tie (extend to next) | (e.g., `1_2` or `1.w_1.w`) |

**Visual Example:**
```ams
1, 1.e, 1.s, 1.h, 1.w
// (Quarter, Eighth, Sixteenth, Half, Whole)
```

### 4.3 Separators (Critical for Readability)

| Symbol | Meaning | Visual Cue |
|--------|---------|------------|
| `,` | Sequential notes (melody) | Read left-to-right → |
| `.` | Simultaneous notes (chord) | Read as vertical stack ↓ |
| `\|\|` | Phrase/chunk boundary | Visual break, implicit sync point |
| `;` | End statement | Hard stop (used in Main) |
| `//` | Comment | Code comment style |

**Visual Logic:**
```ams
1,2,3       // Play: 1 then 2 then 3 (sequence)
1.3.5       // Play: 1 and 3 and 5 together (chord)
1,2 || 3,4  // Play: 1,2 (phrase break) then 3,4
```

### 4.4 Rests

Rests use the same duration suffixes as notes.

| Syntax | Meaning | Beats (in 4/4) |
|--------|---------|----------------|
| `R` | Quarter Rest (default) | 1 beat |
| `R.e` | Eighth Rest | 0.5 beats |
| `R.s` | Sixteenth Rest | 0.25 beats |
| `R.h` | Half Rest | 2 beats |
| `R.w` | Whole Rest | 4 beats |
| `R.` | Dotted Quarter Rest | 1.5 beats |

---

## 5. Segment Structure {#segments}

Segments are the **building blocks** of AMS. Think of them as functions.

### 5.1 Segment Syntax

AMS uses **simple, clean blocks** for hand separation, removing the need for `Begin.*` or `END;`.

```ams
Segment(<index>, <NAME>) {
    Tempo(<bpm>);  // Optional: Override default tempo

    LEFT {
        // Left hand notes (e.g., melody, bass)
    }

    RIGHT {
        // Right hand notes (e.g., chords, harmony)
    }
}
```

### 5.2 Implicit Chunk Alignment (`||`)

AMS uses the `||` separator as an **implicit synchronization point**. This is the core of AMS's visual scanability.

- The parser **automatically aligns** the Nth chunk in `LEFT` with the Nth chunk in `RIGHT`.
- This removes the need for a manual `SYNC()` command.
- It's what a programmer would expect to happen.

**Visual Logic:**
```ams
Segment(1, CHORUS) {
    LEFT {
        1, 2, 3.e, 4.e  ||  5.h
    }

    RIGHT {
        1.3.5           ||  3.5.7.h
    }
}

// HOW IT'S READ:
//
// LEFT:   1, 2, 3.e, 4.e  ||  5.h
// RIGHT:  1.3.5           ||  3.5.7.h
//         └──────┬──────┘   └────┬───┘
//            Chunk 1         Chunk 2
//           (Aligned)       (Aligned)
```

**Note:** If one hand has no notes for a chunk, it is treated as rests.

---

## 6. Control Flow {#control-flow}

AMS uses **function-like invocation** for segments, making composition feel like programming.

### 6.1 Main() Block

The `Main()` block defines **playback order**. Think of it as `main()` in C++.

```ams
Main() {
    Segment(1, INTRO);
    Segment(2, VERSE);
    Segment(3, CHORUS);
}
```

### 6.2 Repeat(n) - Repeat Segments

Use `Repeat(n)` to loop a block of segments `n` times.

```ams
Main() {
    Segment(1, INTRO);

    // Repeat the block 2 times
    Repeat(2) {
        Segment(2, VERSE);
        Segment(3, CHORUS);
    }

    Segment(4, OUTRO);
}

// Execution order:
// INTRO → VERSE → CHORUS → VERSE → CHORUS → OUTRO
```

### 6.3 Inline Notes (Direct Playback)

You can play notes **directly** in `Main()` without defining a segment, useful for short transitions.

```ams
Main() {
    Segment(1, INTRO);

    // Direct notes
    LEFT: 1,2,3,4,5;
    RIGHT: 1.3.5;

    Segment(2, VERSE);
}
```

---

## 7. Macros & Reusability {#macros}

### 7.1 Define (Macros)

Define reusable musical patterns or chords. This is the "programmer-friendly" way to handle music theory.

```ams
// Define a reusable bass line
Define WALTZ_BASS {
    1, 5, 5 || 4, 1, 1 || 5, 2, 2 || 1, 5, 5
}

// Define "theory" as simple constants
Define CHORD_I   { 1.3.5 }
Define CHORD_IV  { 4.6.1 }
Define CHORD_V   { 5.7.2 }
```

### 7.2 Use() (Macro Invocation)

Call a defined macro within any `LEFT` or `RIGHT` block using `Use()`.

```ams
Segment(1, VERSE) {
    LEFT {
        Use(WALTZ_BASS);
    }

    RIGHT {
        Use(CHORD_I.h) || Use(CHORD_IV.h) || Use(CHORD_V.h)
    }
}
```

**Note:** Durations applied to a `Use()` call (like `.h`) apply to the entire defined chunk.

---

## 8. Dynamics & Advanced Features {#advanced-features}

### 8.1 Dynamics (Volume)

Apply dynamics as suffixes to notes.

| Suffix | Meaning | Volume |
|--------|---------|--------|
| `pp` | Pianissimo | Very soft |
| `p` | Piano | Soft |
| `mp` | Mezzo-piano | Moderately soft |
| `mf` | Mezzo-forte | Moderately loud |
| `f` | Forte | Loud |
| `ff` | Fortissimo | Very loud |

**Example:** `1p, 2mp, 3mf, 4f, 5ff`

### 8.2 Articulation

| Suffix | Meaning | Effect |
|--------|---------|--------|
| `!` | Staccato | Short, detached |
| `~` | Legato | Smooth, connected |
| `>` | Accent | Emphasize |

**Example:** `1!, 2!, 3!, 4~, 5~, 6>`

### 8.3 Crescendo / Decrescendo

Wrap a phrase in `<()` or `>()`.

```ams
LEFT {
    <(1, 2, 3, 4, 5)  // Crescendo (get louder)
    >(5, 4, 3, 2, 1)  // Decrescendo (get softer)
}
```

### 8.4 Pedal Notation

```ams
Segment(1, VERSE) {
    RIGHT {
        Pedal.DOWN;
        1.3.5.h, 2.4.6.h, 3.5.7.h
        Pedal.UP;
    }
}
```

### 8.5 Tempo Changes (Ritardando/Accelerando)

```ams
Segment(1, OUTRO) {
    LEFT {
        Tempo(120);
        1, 2, 3, 4, 5
        Ritardando(80);  // Slow down to 80 BPM
        5, 4, 3, 2, 1
    }
}
```

---

## 9. Complete Examples (v1.1 Syntax) {#examples}

### 9.1 "Baa Baa Black Sheep"

```ams
// ============================================
// BAA BAA BLACK SHEEP (v1.1)
// ============================================

Title: "Baa Baa Black Sheep"
Key: C
DefaultTempo: 120
TimeSignature: 4/4

Map { Key: C; Scale: Major; }
Settings { Tempo(120); Octave.LEFT(3); Octave.RIGHT(4); }

// ============================================
// SEGMENTS
// ============================================

Segment(1, VERSE) {
    LEFT {
        // Baa baa black sheep  have you any wool
        1, 1, 5, 5, 6, 6, 5.h      ||  4, 4, 3, 3, 2, 2, 1.h
    }

    RIGHT {
        1.3.5.h                    ||  4.6.1.h, 2.4.6.h
    }
}

Segment(2, CHORUS) {
    LEFT {
        // Yes sir, yes sir,  three bags full
        5, 5, 4, 4, 3, 3, 2.h      ||  5, 5, 4, 4, 3, 3, 2.h
    }

    RIGHT {
        5.7.2.h                    ||  5.7.2.h, 3.5.7.h
    }
}

Segment(3, ENDING) {
    Tempo(100);  // Slow down

    LEFT {
        // One for the little boy  who lives down the lane
        5, 5, 4, 4, 3, 3, 2.h      ||  5, 5, 4, 4, 3, 3, 2.h
    }

    RIGHT {
        5.7.2.h                    ||  5.7.2.h, 1.3.5.w(h)
        //                                            └─ Whole note + Fermata
    }
}

// ============================================
// PLAYBACK ORDER
// ============================================
Main() {
    Segment(1, VERSE);
    Segment(2, CHORUS);
    Segment(1, VERSE); // Re-use segment
    Segment(3, ENDING);
}
```

---

### 9.2 "Happy Birthday"

```ams
// ============================================
// HAPPY BIRTHDAY (v1.1)
// ============================================

Title: "Happy Birthday"
Key: C
DefaultTempo: 120
TimeSignature: 3/4

Map { Key: C; Scale: Major; }
Settings { Tempo(120); Octave.LEFT(3); Octave.RIGHT(4); }

// ============================================
// SEGMENTS
// ============================================

Segment(1, LINE1) {
    LEFT {
        1, 5, 5  ||  1, 5, 5
    }
    RIGHT {
        // Happy birthday to you
        5.e, 5, 6, 5, 1^1, 7.h
    }
}

Segment(2, LINE2) {
    LEFT {
        1, 5, 5  ||  1, 5, 5
    }
    RIGHT {
        // Happy birthday to you
        5.e, 5, 6, 5, 2^1, 1^1.h
    }
}

Segment(3, LINE3) {
    LEFT {
        1, 5, 5  ||  3, 1, 1
    }
    RIGHT {
        // Happy birthday dear [name]
        5.e, 5, 5^1, 3^1, 1^1, 7, 6.h
    }
}

Segment(4, LINE4) {
    Tempo(100);  // Slow down
    LEFT {
        4, 1, 1  ||  5, 1, 1.h
    }
    RIGHT {
        // Happy birthday to you
        4^1.e, 4^1, 3^1, 1^1, 2^1, 1^1.w(h)
    }
}

// ============================================
// PLAYBACK ORDER
// ============================================
Main() {
    Segment(1, LINE1);
    Segment(2, LINE2);
    Segment(3, LINE3);
    Segment(4, LINE4);
}
```

---

### 9.3 Beethoven's "Ode to Joy" (Simplified)

```ams
Title: "Ode to Joy (Simplified) (v1.1)"
Composer: "Ludwig van Beethoven"
Key: D
DefaultTempo: 120

Map { Key: D; Scale: Major; }

Segment(1, THEME_A) {
    RIGHT {
        3, 3, 4, 5    ||  5, 4, 3, 2    ||  1, 1, 2, 3    ||  3., 2.e, 2.h
    }
    LEFT {
        1.3.h         ||  5.7.h         ||  1.3.h         ||  2.4.h
    }
}

Segment(2, THEME_B) {
    RIGHT {
        3, 3, 4, 5    ||  5, 4, 3, 2    ||  1, 1, 2, 3    ||  2., 1.e, 1.h
    }
    LEFT {
        1.3.h         ||  5.7.h         ||  1.3.h         ||  2.4.h, 1.3.h
    }
}

Main() {
    Segment(1, THEME_A);
    Segment(2, THEME_B);
    Segment(1, THEME_A);
}
```

---

### 9.4 "Twinkle Twinkle Little Star"

```ams
Title: "Twinkle Twinkle Little Star"
Key: C
DefaultTempo: 120

Map { Key: C; Scale: Major; }

Segment(1, VERSE) {
    LEFT {
        1, 1, 5, 5, 6, 6, 5.h  ||  4, 4, 3, 3, 2, 2, 1.h
    }

    RIGHT {
        1.3.5.h                ||  4.6.1.h, 2.4.6.h
    }
}

Main() {
    Repeat(2) {
        Segment(1, VERSE);
    }
}
```

---

### 9.5 C Major Scale (Educational)

```ams
Title: "C Major Scale Exercise"
Key: C

Map { Key: C; Scale: Major; }

Segment(1, ASCENDING) {
    Tempo(100);

    RIGHT {
        // C  D  E  F  G  A  B  C
        1, 2, 3, 4, 5, 6, 7, 1^1
    }
}

Segment(2, DESCENDING) {
    Tempo(100);

    RIGHT {
        // C  B  A  G  F  E  D  C
        1^1, 7, 6, 5, 4, 3, 2, 1
    }
}

Main() {
    Repeat(3) {
        Segment(1, ASCENDING);
        Segment(2, DESCENDING);
    }
}
```

---

### 9.6 Simple Waltz (3/4 Time)

```ams
Title: "Simple Waltz"
Key: C
TimeSignature: 3/4

Map { Key: C; Scale: Major; }

Define WALTZ_BASS {
    1, 5, 5 || 4, 1, 1 || 5, 2, 2 || 1, 5, 5
}

Segment(1, WALTZ) {
    Tempo(180);

    LEFT {
        Use(WALTZ_BASS);
    }

    RIGHT {
        1.3.5.h  ||  4.6.1.h  ||  5.7.2.h  ||  1.3.5.h
    }
}

Main() {
    Repeat(4) {
        Segment(1, WALTZ);
    }
}
```

---

## 10. Syntax Reference Table (v1.1) {#syntax-reference}

### 10.1 File Structure

| Element | Syntax | Purpose |
|---------|--------|---------|
| Metadata | `Title: "..."` | Song information |
| Mapping | `Map { ... }` | Note-to-integer mapping |
| Settings | `Settings { ... }` | Global configuration |
| Macro | `Define NAME { ... }` | Define reusable pattern |
| Segment | `Segment(n, NAME) { ... }` | Music block definition |
| Main | `Main() { ... }` | Playback order |

### 10.2 Note Syntax

| Syntax | Meaning | Example |
|--------|---------|---------|
| `1` | Scale degree 1 | C (in C major) |
| `1#` | Sharp | C# |
| `1b` | Flat | Cb |
| `1^1` | One octave up | C5 (if base is C4) |
| `1v_1` | One octave down | C3 |
| `(h)` | Fermata (hold) | `1.w(h)` |
| `.` | Dotted note (1.5x) | `1.` (1.5 beats) |
| `_` | Tie | `1.h_1.e` |

### 10.3 Duration Suffixes

| Suffix | Meaning | Beats (4/4) |
|--------|---------|-------------|
| (none) | Quarter | 1 |
| `.e` | Eighth | 0.5 |
| `.s` | Sixteenth | 0.25 |
| `.h` | Half | 2 |
| `.w` | Whole | 4 |
| `R` | Quarter Rest | 1 |
| `R.e` | Eighth Rest | 0.5 |

### 10.4 Separators & Operators

| Symbol | Meaning | Context |
|--------|---------|---------|
| `,` | Sequential | `1, 2, 3` |
| `.` | Simultaneous | `1.3.5` (chord) |
| `\|\|` | Phrase break | Implicit sync point |
| `;` | Statement end | `Main() { Segment(1); }` |
| `//` | Comment | Code annotation |

### 10.5 Control Flow

| Syntax | Purpose | Example |
|--------|---------|---------|
| `Main() { }` | Entry point | Program execution start |
| `Repeat(n) { }` | Repeat n times | `Repeat(3) { Segment(1); }` |
| `Segment(i, NAME)` | Call segment | `Segment(1, VERSE);` |
| `Use(NAME)` | Call macro | `LEFT { Use(CHORD_I); }` |

### 10.6 Dynamics & Articulation

| Suffix | Meaning | Effect |
|--------|---------|--------|
| `pp` | Pianissimo | Very soft |
| `p` | Piano | Soft |
| `mf` | Mezzo-forte | Moderately loud |
| `f` | Forte | Loud |
| `ff` | Fortissimo | Very loud |
| `!` | Staccato | Short, detached |
| `~` | Legato | Smooth, connected |
| `>` | Accent | Emphasize |

---

## Philosophical Note

```
(˘▾˘)~♪♪

AMS is not about replacing traditional notation.
It's about making music accessible to minds that think differently.

If you can code, you can compose.
If you can debug, you can arrange.
If you can refactor, you can improvise.

Music is patterns.
Code is patterns.
Your brain already knows how to do this.
```

---

## Closing Thoughts

You made this because traditional sheet music didn't work for your brain.

You're not alone.

Thousands of people who can code, debug, build circuits, and design systems
have been told they "can't read music."

But you can read **this**.

And if you can read this, you can play piano.

Welcome to AMS. Your quirky brain is exactly what this language was built for.

```
(ﾉ◕ヮ◕)ﾉ*:･ﾟ✧

Now go make some music.
```

---

**Version:** 1.1  
**Last Updated:** 2025  
**Maintained by:** The Quirky Brain Collective

---

## Quick Reference Card

```
BASIC SYNTAX
  1,2,3       Sequential notes
  1.3.5       Chord (simultaneous)
  ||          Phrase break (implicit sync)
  1.e         Eighth note
  1.h         Half note
  1.w         Whole note
  R           Rest
  1#          Sharp
  1b          Flat
  1^1         Octave up
  1v_1        Octave down
  1p/1f       Soft/Loud

STRUCTURE
  Segment(n, NAME) { }    Define section
  Main() { }              Playback order
  Repeat(n) { }           Repeat n times
  
HANDS
  LEFT { }                Left hand
  RIGHT { }               Right hand
  ||                      Auto-aligns chunks

MAPPING
  Map { Key: C; Scale: Major; }

TEMPO
  Tempo(120);             120 BPM

MACROS
  Define NAME { ... }     Create pattern
  Use(NAME)               Call pattern
```
