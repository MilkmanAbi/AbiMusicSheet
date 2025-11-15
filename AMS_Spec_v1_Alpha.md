# Abi Music Sheet (AMS) Language Specification v1.0

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
6. [Control Flow (Loops & Calls)](#control-flow)
7. [Music Theory Integration](#music-theory)
8. [Advanced Features](#advanced-features)
9. [Complete Examples](#examples)
10. [Syntax Reference Table](#syntax-reference)

---

## 1. Philosophy & Design Goals {#philosophy}

AMS is designed for **programmers, engineers, and analytical minds** who think in syntax, not staves.

### Core Principles:

- **Visual scanability** → Eyes jump between aligned chunks like reading code
- **Minimal cognitive load** → Few symbols, consistent rules  
- **Pattern recognition** → Repetitive structures are obvious at a glance
- **No music theory required** → Integer-based, relative pitch system
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
Composer: "Your Name"
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

// Or custom mapping:
Map {
    1: C;
    2: D;
    3: E;
    4: F;
    5: G;
    6: A;
    7: B;
}

// ============================================
// GLOBAL SETTINGS
// ============================================
Settings {
    Tempo(120);
    Key(C);
    TimeSignature(4/4);
    Octave.LEFT(3);    // Left hand default octave
    Octave.RIGHT(4);   // Right hand default octave
}

// ============================================
// SEGMENT DEFINITIONS (The actual music)
// ============================================
Segment(1, INTRO) { ... }
Segment(2, VERSE) { ... }
Segment(3, CHORUS) { ... }

// ============================================
// MAIN EXECUTION (Playback order)
// ============================================
Main() {
    Segment(1, INTRO);
    Loop(2) {
        Segment(2, VERSE);
        Segment(3, CHORUS);
    }
    Segment(1, INTRO);  // Outro
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

#### Custom Mapping (Any arbitrary mapping):
```ams
Map {
    1: D;
    2: E;
    3: F#;
    4: G;
    5: A;
    6: B;
    7: C#;
}
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
| `^n` | Octave shift up | `1^1` | C one octave up |
| `^-n` | Octave shift down | `5^-1` | G one octave down |
| Combined | Multiple modifiers | `2#^1` | D# one octave up |

### 4.2 Duration Markers

```ams
1       → Quarter note (default, 1 beat)
1'      → Eighth note (0.5 beats)
1''     → Sixteenth note (0.25 beats)
1"      → Half note (2 beats)
1""     → Whole note (4 beats)
1.      → Dotted note (1.5x duration)
1_      → Tied to next note (extend duration)
```

**Examples:**
```ams
1, 2, 3, 4          // Four quarter notes
1', 2', 3', 4'      // Four eighth notes (faster)
1", 2, 3            // Half note followed by two quarters
1., 2', 3           // Dotted quarter, eighth, quarter
```

### 4.3 Separators (Critical for Readability)

| Symbol | Meaning | Visual Cue |
|--------|---------|------------|
| `,` | Sequential notes (melody) | Read left-to-right →  |
| `.` | Simultaneous notes (chord) | Read as vertical stack ↓ |
| `\|\|` | Phrase/chunk boundary | Visual break, new musical idea |
| `;` | End statement | Hard stop |
| `//` | Comment | Code comment style |

**Visual Logic:**
```ams
1,2,3     // Play: 1 then 2 then 3 (sequence)
1.3.5     // Play: 1 and 3 and 5 together (chord)
1,2 || 3,4  // Play: 1,2 (phrase break) then 3,4
```

### 4.4 Rests

```ams
R       → Quarter rest (1 beat silence)
R'      → Eighth rest (0.5 beat silence)
R"      → Half rest (2 beat silence)
R""     → Whole rest (4 beat silence)
```

### 4.5 Dynamics & Expression

**Volume:**
```ams
1pp     → Pianissimo (very soft)
1p      → Piano (soft)
1mp     → Mezzo-piano (moderately soft)
1mf     → Mezzo-forte (moderately loud)
1f      → Forte (loud)
1ff     → Fortissimo (very loud)
```

**Articulation:**
```ams
1!      → Staccato (short, detached)
1~      → Legato (smooth, connected)
1>      → Accent (emphasize)
1^      → Marcato (strong accent)
```

**Crescendo/Decrescendo:**
```ams
<(1,2,3,4,5)    // Crescendo (get louder)
>(5,4,3,2,1)    // Decrescendo (get softer)
```

---

## 5. Segment Structure {#segments}

Segments are the **building blocks** of AMS. Think of them as functions in C++ or methods in Python.

### 5.1 Basic Segment Syntax

```ams
Segment(<index>, <NAME>) {
    Tempo(<bpm>);  // Optional: Override default tempo for this segment
    
    Begin.LEFT {
        // Left hand notes
    }
    
    Begin.RIGHT {
        // Right hand notes
    }
    
    END;
}
```

### 5.2 Simple Example

```ams
Segment(1, INTRO) {
    Tempo(100);
    
    Begin.LEFT {
        1,2,3,4,5
    }
    
    Begin.RIGHT {
        1.3.5, 2.4.6, 3.5.7
    }
    
    END;
}
```

### 5.3 Synchronized Alignment with SYNC()

Use `SYNC()` to **vertically align** LEFT and RIGHT chunks for visual scanning:

```ams
Segment(1, CHORUS) {
    Tempo(120);
    
    Begin.LEFT {
        SYNC();
        1,2,3,4,5  ||  5,4,3,2,1  ||  1",R,R
    }
    
    Begin.RIGHT {
        SYNC();
        1.3.5      ||  3.4.6      ||  1.3.5"
    }
    
    END;
}
```

**Visual Result:**
```
LEFT:   1,2,3,4,5  ||  5,4,3,2,1  ||  1",R,R
RIGHT:  1.3.5      ||  3.4.6      ||  1.3.5"
        └────┬────┘   └────┬────┘   └──┬──┘
          Phrase 1     Phrase 2    Phrase 3
```

### 5.4 Column-Style Formatting (Advanced)

For complex pieces, use **inline comments** as column headers:

```ams
Segment(2, VERSE) {
    Tempo(90);
    
    Begin.LEFT {
        SYNC();
        // Phrase 1        Phrase 2         Phrase 3         Phrase 4
        1,2,3,4,5      ||  5,4,3,2,1    ||  1',1',1',1' ||  1"
    }
    
    Begin.RIGHT {
        SYNC();
        1.3.5          ||  3.5.7        ||  1.4.6       ||  1.3.5"
    }
    
    END;
}
```

---

## 6. Control Flow (Loops & Calls) {#control-flow}

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

### 6.2 Loop() - Repeat Segments

```ams
Loop(<count>) {
    // Segments to repeat
}
```

**Example:**
```ams
Main() {
    Segment(1, INTRO);
    
    Loop(2) {
        Segment(2, VERSE);
        Segment(3, CHORUS);
    }
    
    Segment(4, OUTRO);
}
```

**Execution order:**
```
INTRO → VERSE → CHORUS → VERSE → CHORUS → OUTRO
```

### 6.3 Nested Loops

```ams
Main() {
    Loop(2) {
        Segment(1, VERSE);
        
        Loop(3) {
            Segment(2, CHORUS);
        }
    }
}
```

**Execution:**
```
VERSE → CHORUS → CHORUS → CHORUS → VERSE → CHORUS → CHORUS → CHORUS
```

### 6.4 Inline Notes (Direct Playback)

You can play notes **directly** in `Main()` without defining a segment:

```ams
Main() {
    Segment(1, INTRO);
    
    // Direct notes
    LEFT: 1,2,3,4,5;
    RIGHT: 1.3.5;
    
    Segment(2, VERSE);
}
```

### 6.5 Conditional Playback (Future Feature)

```ams
Main() {
    Segment(1, INTRO);
    
    If(Variation == A) {
        Segment(2, VERSE_A);
    } Else {
        Segment(3, VERSE_B);
    }
}
```

---

## 7. Music Theory Integration {#music-theory}

### 7.1 Scales

AMS supports automatic scale generation:

```ams
Map {
    Key: C;
    Scale: Major;
}
// Generates: C, D, E, F, G, A, B (1-7)

Map {
    Key: A;
    Scale: Minor;
}
// Generates: A, B, C, D, E, F, G (1-7)

Map {
    Key: G;
    Scale: Pentatonic;
}
// Generates: G, A, B, D, E (1-5)
```

### 7.2 Chord Notation

AMS uses **Roman numeral analysis** for chord progressions:

```ams
Define PROGRESSION_I_IV_V {
    I, IV, V, I
}

// Maps to:
// I   = 1.3.5  (C major: C-E-G)
// IV  = 4.6.1  (C major: F-A-C)
// V   = 5.7.2  (C major: G-B-D)
```

**Chord Types:**
```ams
I       → Major triad (1.3.5)
i       → Minor triad (1.3b.5)
Imaj7   → Major 7th (1.3.5.7)
I7      → Dominant 7th (1.3.5.7b)
idim    → Diminished (1.3b.5b)
Iaug    → Augmented (1.3.5#)
```

### 7.3 Intervals

```ams
// Interval notation
1+3     → Major third above 1 (1 to 3)
1+5     → Perfect fifth above 1 (1 to 5)
5-2     → Major third below 5 (5 to 3)
```

### 7.4 Key Signatures

```ams
Map {
    Key: D;
    Scale: Major;
    Sharps: [F#, C#];  // Auto-applied to all notes
}
```

### 7.5 Time Signatures

```ams
Settings {
    TimeSignature(3/4);  // Waltz time
}

Settings {
    TimeSignature(6/8);  // Compound meter
}

Settings {
    TimeSignature(7/8);  // Odd meter
}
```

### 7.6 Modes

```ams
Map {
    Key: D;
    Mode: Dorian;
}
// Generates: D, E, F, G, A, B, C

Map {
    Key: E;
    Mode: Phrygian;
}
// Generates: E, F, G, A, B, C, D
```

---

## 8. Advanced Features {#advanced-features}

### 8.1 Macros (Reusable Patterns)

Define reusable musical patterns:

```ams
Define ARPEGGIO_UP {
    1,3,5,3,1
}

Define WALTZ_BASS {
    1, 5, 5 || 4, 1, 1 || 5, 2, 2 || 1, 5, 5
}

Segment(1, VERSE) {
    Begin.LEFT {
        Use(WALTZ_BASS);
    }
    
    BEGIN.RIGHT {
        Use(ARPEGGIO_UP);
    }
    
    END;
}
```

### 8.2 Variations

Create variations of segments:

```ams
Segment(1, CHORUS) {
    Begin.LEFT {
        1,2,3,4,5
    }
    
    Variation.FAST {
        Begin.LEFT {
            1',2',3',4',5'  // Faster version
        }
    }
    
    Variation.SOFT {
        Begin.LEFT {
            1p,2p,3p,4p,5p  // Softer version
        }
    }
    
    END;
}

Main() {
    Segment(1, CHORUS);
    Segment(1, CHORUS).Variation.FAST;
    Segment(1, CHORUS).Variation.SOFT;
}
```

### 8.3 Pedal Notation

```ams
Segment(1, VERSE) {
    Pedal.DOWN;
    
    Begin.RIGHT {
        1.3.5", 2.4.6", 3.5.7"
    }
    
    Pedal.UP;
    
    END;
}
```

### 8.4 Hand Position Markers

```ams
Segment(1, INTRO) {
    Begin.LEFT {
        Position(C2);  // Position left hand at C2
        1,2,3,4,5
    }
    
    Begin.RIGHT {
        Position(C4);  // Position right hand at C4
        5,4,3,2,1
    }
    
    END;
}
```

### 8.5 Tempo Changes (Ritardando/Accelerando)

```ams
Segment(1, OUTRO) {
    Begin.LEFT {
        Tempo(120);
        1,2,3,4,5
        Ritardando(80);  // Slow down to 80 BPM
        5,4,3,2,1
    }
    
    END;
}
```

### 8.6 Fermata (Hold)

```ams
Segment(1, ENDING) {
    Begin.LEFT {
        1,2,3,4,5^  // ^ = fermata (hold this note)
    }
    
    END;
}
```

---

## 9. Complete Examples {#examples}

### 9.1 "Baa Baa Black Sheep" (Full Score)

```ams
// ============================================
// BAA BAA BLACK SHEEP
// Traditional Nursery Rhyme
// ============================================

Title: "Baa Baa Black Sheep"
Artist: "Traditional"
Composer: "Traditional"
Key: C
DefaultTempo: 120
TimeSignature: 4/4

// ============================================
// NOTE MAPPING
// ============================================
Map {
    Key: C;
    Scale: Major;
    // Generated mapping:
    // 1 = C (Baa)
    // 2 = D (Baa)
    // 3 = E (Black)
    // 4 = F (Sheep)
    // 5 = G (Have)
    // 6 = A (You)
    // 7 = B (Any)
}

// ============================================
// SETTINGS
// ============================================
Settings {
    Tempo(120);
    Octave.LEFT(3);
    Octave.RIGHT(4);
}

// ============================================
// SEGMENTS
// ============================================

Segment(1, VERSE) {
    Tempo(120);
    
    Begin.LEFT {
        SYNC();
        // Baa baa black sheep  have you any wool
        1,1,5,5,6,6,5"    ||  4,4,3,3,2,2,1"
    }
    
    Begin.RIGHT {
        SYNC();
        1.3.5"            ||  4.6.1", 2.4.6"
    }
    
    END;
}

Segment(2, CHORUS) {
    Tempo(120);
    
    Begin.LEFT {
        SYNC();
        // Yes sir, yes sir,  three bags full
        5,5,4,4,3,3,2"  ||  5,5,4,4,3,3,2"
    }
    
    Begin.RIGHT {
        SYNC();
        5.7.2"          ||  5.7.2", 3.5.7"
    }
    
    END;
}

Segment(3, BRIDGE) {
    Tempo(120);
    
    Begin.LEFT {
        SYNC();
        // One for my master,  one for my dame
        1,1,5,5,6,6,5"   ||  4,4,3,3,2,2,1"
    }
    
    Begin.RIGHT {
        SYNC();
        1.3.5"           ||  4.6.1", 2.4.6"
    }
    
    END;
}

Segment(4, ENDING) {
    Tempo(100);  // Slow down for ending
    
    Begin.LEFT {
        SYNC();
        // One for the little boy  who lives down the lane
        5,5,4,4,3,3,2"        ||  5,5,4,4,3,3,2"
    }
    
    Begin.RIGHT {
        SYNC();
        5.7.2"                ||  5.7.2", 1.3.5""^
        //                                    └─ Fermata (hold)
    }
    
    END;
}

// ============================================
// PLAYBACK ORDER
// ============================================
Main() {
    Segment(1, VERSE);
    Segment(2, CHORUS);
    Segment(3, BRIDGE);
    Segment(4, ENDING);
}
```

---

### 9.2 "Twinkle Twinkle Little Star"

```ams
Title: "Twinkle Twinkle Little Star"
Key: C
DefaultTempo: 120

Map {
    Key: C;
    Scale: Major;
}

Settings {
    Tempo(120);
    Octave.RIGHT(4);
}

Segment(1, VERSE) {
    Tempo(120);
    
    Begin.LEFT {
        SYNC();
        1,1,5,5,6,6,5" || 4,4,3,3,2,2,1"
    }
    
    Begin.RIGHT {
        SYNC();
        1.3.5"         || 4.6.1", 2.4.6"
    }
    
    END;
}

Main() {
    Loop(2) {
        Segment(1, VERSE);
    }
}
```

---

### 9.3 C Major Scale (Educational)

```ams
Title: "C Major Scale Exercise"
Key: C

Map {
    Key: C;
    Scale: Major;
}

Segment(1, ASCENDING) {
    Tempo(100);
    
    Begin.RIGHT {
        // C  D  E  F  G  A  B  C
        1, 2, 3, 4, 5, 6, 7, 1^1
    }
    
    END;
}

Segment(2, DESCENDING) {
    Tempo(100);
    
    Begin.RIGHT {
        // C  B  A  G  F  E  D  C
        1^1, 7, 6, 5, 4, 3, 2, 1
    }
    
    END;
}

Main() {
    Loop(3) {
        Segment(1, ASCENDING);
        Segment(2, DESCENDING);
    }
}
```

---

### 9.4 Simple Chord Progression (I-IV-V-I)

```ams
Title: "Basic Chord Progression"
Key: C

Map {
    Key: C;
    Scale: Major;
}

Segment(1, PROGRESSION) {
    Tempo(80);
    
    Begin.LEFT {
        SYNC();
        1"  ||  4"  ||  5"  ||  1"
    }
    
    Begin.RIGHT {
        SYNC();
        1.3.5"  ||  4.6.1"  ||  5.7.2"  ||  1.3.5""
    }
    
    END;
}

Main() {
    Loop(4) {
        Segment(1, PROGRESSION);
    }
}
```

---

### 9.5 Waltz Pattern (3/4 Time)

```ams
Title: "Simple Waltz"
Key: C
TimeSignature: 3/4

Map {
    Key: C;
    Scale: Major;
}

Define WALTZ_BASS {
    1, 5, 5 || 4, 1, 1 || 5, 2, 2 || 1, 5, 5
}

Segment(1, WALTZ) {
    Tempo(180);
    
    Begin.LEFT {
        SYNC();
        Use(WALTZ_BASS);
    }
    
    Begin.RIGHT {
        SYNC();
        1.3.5"  ||  4.6.1"  ||  5.7.2"  ||  1.3.5"
    }
    
    END;
}

Main() {
    Loop(4) {
        Segment(1, WALTZ);
    }
}
```

---

### 9.6 Advanced: Beethoven's "Ode to Joy" (Simplified)

```ams
Title: "Ode to Joy (Simplified)"
Composer: "Ludwig van Beethoven"
Key: D

Map {
    Key: D;
    Scale: Major;
}

Settings {
    Tempo(120);
}

Segment(1, THEME_A) {
    Tempo(120);
    
    Begin.RIGHT {
        SYNC();
        3,3,4,5  ||  5,4,3,2  ||  1,1,2,3  ||  3.,2',2"
    }
    
    Begin.LEFT {
        SYNC();
        1.3"     ||  5.7"     ||  1.3"     ||  2.4"
    }
    
    END;
}

Segment(2, THEME_B) {
    Tempo(120);
    
    Begin.RIGHT {
        SYNC();
        3,3,4,5  ||  5,4,3,2  ||  1,1,2,3  ||  2.,1',1"
    }
    
    Begin.LEFT {
        SYNC();
        1.3"     ||  5.7"     ||  1.3"     ||  2.4", 1.3"
    }
    
    END;
}

Main() {
    Segment(1, THEME_A);
    Segment(2, THEME_B);
    Segment(1, THEME_A);
}
```

---

## 10. Syntax Reference Table {#syntax-reference}

### 10.1 File Structure Elements

| Element | Syntax | Purpose |
|---------|--------|---------|
| Metadata | `Title: "..."` | Song information |
| Mapping | `Map { Key: C; Scale: Major; }` | Note-to-integer mapping |
| Settings | `Settings { Tempo(120); }` | Global configuration |
| Segment | `Segment(1, NAME) { ... }` | Music block definition |
| Main | `Main() { ... }` | Playback order |

### 10.2 Note Syntax

| Syntax | Meaning | Example |
|--------|---------|---------|
| `1` | Scale degree 1 | C (in C major) |
| `1#` | Sharp | C# |
| `1b` | Flat | Cb |
| `1^1` | One octave up | C5 (if base is C4) |
| `1^-1` | One octave down | C3 |
| `1'` | Eighth note | Short duration |
| `1"` | Half note | Long duration |
| `1.` | Dotted note | 1.5x duration |
| `1_` | Tie | Extend to next note |

### 10.3 Separators & Operators

| Symbol | Meaning | Context |
|--------|---------|---------|
| `,` | Sequential | Play notes one after another |
| `.` | Simultaneous | Play notes together (chord) |
| `\|\|` | Phrase break | Visual/musical separation |
| `;` | Statement end | End of block |
| `//` | Comment | Code annotation |

### 10.4 Control Flow

| Syntax | Purpose | Example |
|--------|---------|---------|
| `Main() { }` | Entry point | Program execution start |
| `Loop(n) { }` | Repeat n times | `Loop(3) { Segment(1); }` |
| `Segment(i, NAME)` | Call segment | `Segment(1, VERSE);` |
| `If/Else` | Conditional | Future feature |

### 10.5 Music Theory

| Element | Syntax | Example |
|---------|--------|---------|
| Tempo | `Tempo(120)` | 120 BPM |
| Key | `Key: C` | C major/minor |
| Scale | `Scale: Major` | Major scale |
| Time Signature | `TimeSignature(4/4)` | Common time |
| Chord | `I, IV, V` | Roman numerals |

### 10.6 Dynamics

| Symbol | Meaning | Volume |
|--------|---------|--------|
| `pp` | Pianissimo | Very soft |
| `p` | Piano | Soft |
| `mp` | Mezzo-piano | Moderately soft |
| `mf` | Mezzo-forte | Moderately loud |
| `f` | Forte | Loud |
| `ff` | Fortissimo | Very loud |

### 10.7 Articulation

| Symbol | Meaning | Effect |
|--------|---------|--------|
| `!` | Staccato | Short, detached |
| `~` | Legato | Smooth, connected |
| `>` | Accent | Emphasize |
| `^` | Fermata | Hold/pause |
| `<()` | Crescendo | Get louder |
| `>()` | Decrescendo | Get softer |

---

## Appendix: Design Philosophy

### Why AMS Exists

Traditional sheet music is optimized for:
- Musicians trained in classical notation
- Visual representation of pitch and rhythm
- Historical conventions dating back centuries

AMS is optimized for:
- **Programmers** who think in syntax and structure
- **Pattern recognition** through code-like formatting
- **Version control** (plain text, git-friendly)
- **Modular composition** (segments as functions)
- **Rapid prototyping** (write music as fast as you code)

### Reading AMS Like Code

```
(｀・ω・´)ゞ

When you see:
  1,2,3,4,5  →  Think: for loop iterating up
  5,4,3,2,1  →  Think: for loop iterating down
  1.3.5      →  Think: struct with three fields
  ||         →  Think: logical OR / section break
  Loop(3)    →  Think: while(i < 3)
```

### The Quirky Brain Advantage

AMS embraces the way **analytical minds** process information:
- **Chunking** → Visual breaks with `||`
- **Patterns** → Repetition is obvious
- **Structure** → Hierarchical like code
- **Logic** → Predictable, consistent rules
- **Modularity** → Reuse segments like functions
- **Clarity** → No ambiguity, no interpretation needed

---

## Advanced Topics

### Polymorphism in Music (Segment Overloading)

You can define multiple versions of the same segment and call them conditionally:

```ams
Segment(1, VERSE).Version.A {
    Begin.LEFT {
        1,2,3,4,5
    }
    END;
}

Segment(1, VERSE).Version.B {
    Begin.LEFT {
        5,4,3,2,1
    }
    END;
}

Main() {
    Segment(1, VERSE).Version.A;
    Segment(1, VERSE).Version.B;
}
```

### Algorithmic Composition

Generate patterns programmatically:

```ams
Define ASCENDING_PATTERN(start, end) {
    Range(start, end, step=1)  // Generates: start, start+1, ..., end
}

Segment(1, EXERCISE) {
    Begin.RIGHT {
        Use(ASCENDING_PATTERN(1, 8));  // 1,2,3,4,5,6,7,8
    }
    END;
}
```

### Transposition

Shift all notes by a specific interval:

```ams
Segment(1, MELODY) {
    Begin.RIGHT {
        1,2,3,4,5
    }
    END;
}

Segment(2, MELODY_TRANSPOSED) {
    Transpose(+4);  // Shift up by 4 scale degrees
    Begin.RIGHT {
        Use(Segment(1, MELODY).RIGHT);  // Reuse with transposition
    }
    END;
}
```

### Inversions (Chord Voicing)

```ams
Define TRIAD_ROOT {
    1.3.5  // Root position
}

Define TRIAD_FIRST_INV {
    3.5.1^1  // First inversion
}

Define TRIAD_SECOND_INV {
    5.1^1.3^1  // Second inversion
}

Segment(1, VOICING_DEMO) {
    Begin.RIGHT {
        Use(TRIAD_ROOT);
        Use(TRIAD_FIRST_INV);
        Use(TRIAD_SECOND_INV);
    }
    END;
}
```

### Voice Leading

Define voice leading rules for smooth transitions:

```ams
Settings {
    VoiceLeading: Smooth;  // Minimize interval jumps
}

Segment(1, PROGRESSION) {
    Begin.RIGHT {
        1.3.5  →  1.4.6  →  2.5.7  →  1.3.5
        // → indicates voice leading connection
    }
    END;
}
```

### Arpeggiation

```ams
Define ARPEGGIO(chord, pattern) {
    // Breaks chord into pattern
    // pattern: "up", "down", "up-down", "random"
}

Segment(1, BROKEN_CHORD) {
    Begin.RIGHT {
        Arpeggio(1.3.5, "up");      // 1,3,5
        Arpeggio(2.4.6, "down");    // 6,4,2
        Arpeggio(3.5.7, "up-down"); // 3,5,7,5,3
    }
    END;
}
```

---

## Extended Example: "Happy Birthday" (Complete)

```ams
// ============================================
// HAPPY BIRTHDAY
// Traditional Birthday Song
// ============================================

Title: "Happy Birthday"
Artist: "Traditional"
Key: C
DefaultTempo: 120
TimeSignature: 3/4

// ============================================
// NOTE MAPPING
// ============================================
Map {
    Key: C;
    Scale: Major;
    // 1=C, 2=D, 3=E, 4=F, 5=G, 6=A, 7=B
}

// ============================================
// SETTINGS
// ============================================
Settings {
    Tempo(120);
    Octave.LEFT(3);
    Octave.RIGHT(4);
}

// ============================================
// MACROS
// ============================================
Define BIRTHDAY_BASS {
    1, 5, 5 || 1, 5, 5 || 1, 5, 5 || 4, 1, 1
}

// ============================================
// SEGMENTS
// ============================================

Segment(1, LINE1) {
    Tempo(120);
    
    Begin.LEFT {
        SYNC();
        1, 5, 5  ||  1, 5, 5
    }
    
    Begin.RIGHT {
        SYNC();
        // Happy birthday to you
        5', 5, 6, 5, 1^1, 7"
    }
    
    END;
}

Segment(2, LINE2) {
    Tempo(120);
    
    Begin.LEFT {
        SYNC();
        1, 5, 5  ||  1, 5, 5
    }
    
    Begin.RIGHT {
        SYNC();
        // Happy birthday to you
        5', 5, 6, 5, 2^1, 1^1"
    }
    
    END;
}

Segment(3, LINE3) {
    Tempo(120);
    
    Begin.LEFT {
        SYNC();
        1, 5, 5  ||  3, 1, 1
    }
    
    Begin.RIGHT {
        SYNC();
        // Happy birthday dear [name]
        5', 5, 5^1, 3^1, 1^1, 7, 6"
    }
    
    END;
}

Segment(4, LINE4) {
    Tempo(100);  // Slow down for ending
    
    Begin.LEFT {
        SYNC();
        4, 1, 1  ||  5, 1, 1"
    }
    
    Begin.RIGHT {
        SYNC();
        // Happy birthday to you
        4^1', 4^1, 3^1, 1^1, 2^1, 1^1""^
        //                             └─ Fermata
    }
    
    END;
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

## Extended Example: Chopin's "Prelude in E Minor" (Simplified)

```ams
// ============================================
// PRELUDE IN E MINOR, OP. 28 NO. 4
// Frédéric Chopin (Simplified)
// ============================================

Title: "Prelude in E Minor"
Composer: "Frédéric Chopin"
Key: E
DefaultTempo: 50
TimeSignature: 4/4

Map {
    Key: E;
    Scale: Minor;
}

Settings {
    Tempo(50);  // Very slow, expressive
    Octave.LEFT(2);
    Octave.RIGHT(4);
}

// ============================================
// THEME A - Opening
// ============================================
Segment(1, OPENING) {
    Tempo(50);
    
    Begin.LEFT {
        SYNC();
        // Slow chord progression
        Pedal.DOWN;
        1"  ||  7b^-1"  ||  6^-1"  ||  5^-1"
        Pedal.UP;
    }
    
    Begin.RIGHT {
        SYNC();
        // Descending melodic line (molto espressivo)
        1p, 7bp, 1p, 7bp  ||  6p, 5p, 6p, 5p  ||  4p, 3p, 4p, 3p  ||  2p, 1p, 2p, 1p
    }
    
    END;
}

Segment(2, DEVELOPMENT) {
    Tempo(48);  // Even slower
    
    Begin.LEFT {
        SYNC();
        Pedal.DOWN;
        4^-1"  ||  3^-1"  ||  2^-1"  ||  1^-1"
        Pedal.UP;
    }
    
    Begin.RIGHT {
        SYNC();
        7bp, 6p, 7bp, 6p  ||  5p, 4p, 5p, 4p  ||  3pp, 2pp, 3pp, 2pp  ||  1pp"
    }
    
    END;
}

Segment(3, CLIMAX) {
    Tempo(52);
    
    Begin.LEFT {
        SYNC();
        Pedal.DOWN;
        5^-1"  ||  4^-1"  ||  3^-1"  ||  2^-1"
    }
    
    Begin.RIGHT {
        SYNC();
        5mf>, 4mf>, 5mf>, 4mf>  ||  3f>, 2f>, 3f>, 2f>  ||  1ff>""
    }
    
    END;
}

Segment(4, RESOLUTION) {
    Tempo(45);  // Ritardando
    
    Begin.LEFT {
        SYNC();
        Pedal.DOWN;
        1^-1"  ||  1^-1""^
        Pedal.UP;
    }
    
    Begin.RIGHT {
        SYNC();
        1pp, 7bpp, 1pp, 7bpp  ||  1ppp""^
    }
    
    END;
}

Main() {
    Segment(1, OPENING);
    Segment(2, DEVELOPMENT);
    Segment(3, CLIMAX);
    Segment(4, RESOLUTION);
}
```

---

## Practical Workflow

### Step 1: Hear the Song
Listen to the piece you want to transcribe. Identify:
- Melody (usually left hand)
- Harmony/chords (usually right hand)
- Bass line (usually left hand)
- Repeating patterns

### Step 2: Map the Notes
Create your `Map` block:
```ams
Map {
    Key: C;  // What key is the song in?
    Scale: Major;  // Major or Minor?
}
```

### Step 3: Break into Segments
Divide the song into logical sections:
- INTRO
- VERSE
- CHORUS
- BRIDGE
- OUTRO

### Step 4: Transcribe by Ear
For each segment:
1. Hum/sing the melody
2. Find the first note on piano → write it down
3. Find the next note → is it higher or lower?
4. Write the sequence: `1,2,3` or `5,4,3`

### Step 5: Add Chords
Listen for chord changes:
1. Play the melody
2. Find which chord sounds right underneath
3. Common chords: 1.3.5 (I), 4.6.1 (IV), 5.7.2 (V)

### Step 6: Sync and Align
Use `SYNC()` to line up melody with chords:
```ams
Begin.LEFT {
    SYNC();
    1,2,3,4,5  ||  5,4,3,2,1
}

Begin.RIGHT {
    SYNC();
    1.3.5      ||  5.7.2
}
```

### Step 7: Add Expression
- Dynamics: `1p` (soft), `1f` (loud)
- Duration: `1'` (short), `1"` (long)
- Articulation: `1!` (staccato), `1~` (legato)

### Step 8: Structure the Playback
```ams
Main() {
    Segment(1, INTRO);
    Loop(2) {
        Segment(2, VERSE);
        Segment(3, CHORUS);
    }
    Segment(4, OUTRO);
}
```

---

## Tools & Ecosystem (Future)

### AMS Compiler
Converts AMS files to:
- MIDI files (for playback in DAWs)
- Sheet music PDF (for traditional musicians)
- Audio files (synthesized piano)

### AMS Editor (Web-based)
- Syntax highlighting
- Live preview (hear as you type)
- Auto-complete for segments
- Visual keyboard overlay

### AMS Player
- Command-line tool: `ams-play song.ams`
- Tempo control
- Loop sections for practice
- Slow down/speed up without pitch change

### AMS Library
Community-contributed songs:
```bash
ams-get "happy-birthday"
ams-get "fur-elise"
ams-get "claire-de-lune"
```

### Version Control Integration
```bash
git init my-music
ams create intro.ams
git add intro.ams
git commit -m "Added intro segment"
git diff intro.ams  # See what changed in your music!
```

---

## FAQ

**Q: How do I know which number corresponds to which note?**
A: Check your `Map` block. If `Key: C` and `Scale: Major`, then 1=C, 2=D, 3=E, 4=F, 5=G, 6=A, 7=B.

**Q: What if I don't know music theory?**
A: You don't need to! Just:
1. Play notes on the piano
2. Write down if the next note is higher (bigger number) or lower (smaller number)
3. Use your ears

**Q: How do I transcribe a song I only know by ear?**
A: 
1. Find the first note by trial and error
2. Sing the melody
3. For each note, ask: "Is it higher or lower than the previous?"
4. Write it down: `1,2,3` (going up) or `5,4,3` (going down)

**Q: Can I use AMS for anything other than piano?**
A: Yes! The system works for any melodic instrument. Just adjust the `Octave` settings.

**Q: What about rhythm? How do I know if a note is short or long?**
A: 
- Default (no apostrophe) = normal length
- `'` = short (half the length)
- `"` = long (double the length)
- Listen and match what you hear

**Q: How do I handle black keys?**
A: Use `#` for sharp or `b` for flat: `1#` = C#, `3b` = Eb

**Q: Can I share my AMS files?**
A: Yes! They're plain text. Share via GitHub, Discord, email, etc.

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

## Contributing to AMS

AMS is open for community development. Contribute by:

1. **Adding new notation features**
2. **Creating transcriptions** of popular songs
3. **Building tools** (parsers, players, editors)
4. **Improving documentation**
5. **Sharing your music** written in AMS

---

## License

AMS Language Specification v1.0
Created by Abi (Engineering Student, Plant Enthusiast, Programmer)

Free to use, modify, and distribute.
Attribution appreciated but not required.

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

**Version:** 1.0  
**Last Updated:** 2025  
**Maintained by:** The Quirky Brain Collective  
**Contact:** [Your preferred contact method]

---

## Quick Reference Card

```
BASIC SYNTAX
  1,2,3       Sequential notes
  1.3.5       Chord (simultaneous)
  ||          Phrase break
  1'          Short note
  1"          Long note
  R           Rest
  1#          Sharp
  1b          Flat
  1^1         Octave up
  1p/1f       Soft/Loud

STRUCTURE
  Segment(n, NAME) { }    Define section
  Main() { }              Playback order
  Loop(n) { }             Repeat n times
  
HANDS
  Begin.LEFT { }          Left hand
  Begin.RIGHT { }         Right hand
  SYNC();                 Align visually

MAPPING
  Map { Key: C; Scale: Major; }

TEMPO
  Tempo(120);             120 BPM
```
