#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <regex>
#include <algorithm>
#include <cctype>
#include <cstdint>

// ============================================
// MIDI File Writer
// ============================================
class MIDIWriter {
private:
    std::vector<uint8_t> data;
    
    void writeBytes(const std::vector<uint8_t>& bytes) {
        data.insert(data.end(), bytes.begin(), bytes.end());
    }
    
    void writeByte(uint8_t byte) {
        data.push_back(byte);
    }
    
    void writeInt16(uint16_t value) {
        data.push_back((value >> 8) & 0xFF);
        data.push_back(value & 0xFF);
    }
    
    void writeInt32(uint32_t value) {
        data.push_back((value >> 24) & 0xFF);
        data.push_back((value >> 16) & 0xFF);
        data.push_back((value >> 8) & 0xFF);
        data.push_back(value & 0xFF);
    }
    
    void writeVarLen(uint32_t value) {
        if (value >= 0x10000000) {
            writeByte(0x80 | ((value >> 28) & 0x7F));
        }
        if (value >= 0x200000) {
            writeByte(0x80 | ((value >> 21) & 0x7F));
        }
        if (value >= 0x4000) {
            writeByte(0x80 | ((value >> 14) & 0x7F));
        }
        if (value >= 0x80) {
            writeByte(0x80 | ((value >> 7) & 0x7F));
        }
        writeByte(value & 0x7F);
    }
    
    void writeString(const std::string& str) {
        for (char c : str) {
            writeByte(static_cast<uint8_t>(c));
        }
    }

public:
    void writeHeader(uint16_t format, uint16_t num_tracks, uint16_t division) {
        writeString("MThd");
        writeInt32(6);
        writeInt16(format);
        writeInt16(num_tracks);
        writeInt16(division);
    }
    
    void startTrack() {
        writeString("MTrk");
        writeInt32(0);
    }
    
    size_t getTrackLengthPosition() {
        return data.size() - 4;
    }
    
    void updateTrackLength(size_t pos, uint32_t length) {
        data[pos] = (length >> 24) & 0xFF;
        data[pos + 1] = (length >> 16) & 0xFF;
        data[pos + 2] = (length >> 8) & 0xFF;
        data[pos + 3] = length & 0xFF;
    }
    
    void writeDeltaTime(uint32_t delta) {
        writeVarLen(delta);
    }
    
    void writeMetaEvent(uint8_t type, const std::vector<uint8_t>& data) {
        writeByte(0xFF);
        writeByte(type);
        writeVarLen(data.size());
        writeBytes(data);
    }
    
    void writeNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
        writeByte(0x90 | channel);
        writeByte(note);
        writeByte(velocity);
    }
    
    void writeNoteOff(uint8_t channel, uint8_t note) {
        writeByte(0x80 | channel);
        writeByte(note);
        writeByte(64);
    }
    
    void writeProgramChange(uint8_t channel, uint8_t program) {
        writeByte(0xC0 | channel);
        writeByte(program);
    }
    
    void writeTempoChange(uint32_t microseconds_per_quarter) {
        std::vector<uint8_t> tempo_data = {
            static_cast<uint8_t>((microseconds_per_quarter >> 16) & 0xFF),
            static_cast<uint8_t>((microseconds_per_quarter >> 8) & 0xFF),
            static_cast<uint8_t>(microseconds_per_quarter & 0xFF)
        };
        writeMetaEvent(0x51, tempo_data);
    }
    
    void writeTimeSignature(uint8_t numerator, uint8_t denominator) {
        uint8_t denom_log2 = 0;
        uint8_t temp = denominator;
        while (temp > 1) {
            temp >>= 1;
            denom_log2++;
        }
        
        std::vector<uint8_t> ts_data = {numerator, denom_log2, 24, 8};
        writeMetaEvent(0x58, ts_data);
    }
    
    void writeTrackName(const std::string& name) {
        std::vector<uint8_t> name_data(name.begin(), name.end());
        writeMetaEvent(0x03, name_data);
    }
    
    void writeEndOfTrack() {
        writeMetaEvent(0x2F, {});
    }
    
    bool writeToFile(const std::string& filename) {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) return false;
        
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        file.close();
        return true;
    }
    
    size_t size() const { return data.size(); }
};

// ============================================
// MIDI Note Conversion
// ============================================
class MIDINoteConverter {
private:
    std::map<std::string, int> note_to_midi = {
        {"C", 0}, {"C#", 1}, {"Db", 1},
        {"D", 2}, {"D#", 3}, {"Eb", 3},
        {"E", 4}, 
        {"F", 5}, {"F#", 6}, {"Gb", 6},
        {"G", 7}, {"G#", 8}, {"Ab", 8},
        {"A", 9}, {"A#", 10}, {"Bb", 10},
        {"B", 11}
    };

public:
    int pitchToMIDI(const std::string& pitch, int octave, int octave_shift = 0) {
        if (note_to_midi.find(pitch) == note_to_midi.end()) {
            return 60;
        }
        
        int base_note = note_to_midi[pitch];
        int total_octave = octave + octave_shift;
        return (total_octave + 1) * 12 + base_note;
    }
    
    int velocityFromDynamic(const std::string& dynamic) {
        if (dynamic == "pp") return 40;
        if (dynamic == "p") return 60;
        if (dynamic == "mp") return 75;
        if (dynamic == "mf") return 90;
        if (dynamic == "f") return 105;
        if (dynamic == "ff") return 120;
        return 90;
    }
};

// ============================================
// Data Structures
// ============================================
struct Note {
    int degree;
    std::string accidental;
    int octave_shift;
    double duration;
    bool is_dotted;
    std::string articulation;
    std::string dynamic;
    bool is_rest;

    Note() : degree(0), octave_shift(0), duration(1.0), is_dotted(false), is_rest(false) {}
};

struct Chord {
    std::vector<Note> notes;
    double duration;
    bool is_dotted;

    Chord() : duration(1.0), is_dotted(false) {}
};

struct Hand {
    std::vector<std::vector<Chord>> chunks;
};

struct Segment {
    int id;
    std::string name;
    int tempo;
    Hand left;
    Hand right;
    int definition_line;
};

struct Metadata {
    std::string title;
    std::string composer;
    std::string key;
    int tempo;
    std::string time_signature;
    int difficulty;
};

struct MapBlock {
    std::string key;
    std::string scale;
    std::map<int, std::string> note_mapping;
    bool defined;
    int line_number;
    
    MapBlock() : defined(false), line_number(0) {}
};

// ============================================
// Utility Functions
// ============================================
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

std::vector<std::string> split(const std::string& str, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delim)) {
        result.push_back(trim(item));
    }
    return result;
}

bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix;
}

std::string removeComments(const std::string& line) {
    size_t pos = line.find("//");
    return (pos != std::string::npos) ? line.substr(0, pos) : line;
}

std::string replaceExtension(const std::string& filename, const std::string& new_ext) {
    size_t last_dot = filename.find_last_of('.');
    size_t last_slash = filename.find_last_of("/\\");
    
    if (last_dot == std::string::npos || (last_slash != std::string::npos && last_dot < last_slash)) {
        return filename + new_ext;
    }
    
    return filename.substr(0, last_dot) + new_ext;
}

// ============================================
// Parser Class
// ============================================
class AMSParser {
private:
    std::vector<std::string> lines;
    std::vector<std::string> original_lines;
    size_t current_line;
    Metadata metadata;
    MapBlock map_block;
    std::vector<Segment> segments;
    std::vector<std::string> errors;

public:
    AMSParser(const std::string& filename) : current_line(0) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            errors.push_back("ERROR: Cannot open file: " + filename);
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            original_lines.push_back(line);
            line = removeComments(line);
            line = trim(line);
            lines.push_back(line.empty() ? "" : line);
        }
    }

    bool parse() {
        parseMetadata();
        if (!parseMap()) return false;
        generateNoteMapping();
        
        while (current_line < lines.size()) {
            if (lines[current_line].empty()) {
                current_line++;
                continue;
            }
            
            if (startsWith(lines[current_line], "Segment(")) {
                parseSegment();
            } else if (startsWith(lines[current_line], "Main()")) {
                break;
            } else {
                current_line++;
            }
        }
        
        return !segments.empty();
    }

    const Metadata& getMetadata() const { return metadata; }
    const MapBlock& getMapBlock() const { return map_block; }
    const std::vector<Segment>& getSegments() const { return segments; }
    
    bool hasErrors() const { return !errors.empty(); }
    void printErrors() const {
        for (const auto& err : errors) {
            std::cerr << err << std::endl;
        }
    }

private:
    void parseMetadata() {
        metadata.tempo = 120;
        metadata.difficulty = 0;
        metadata.time_signature = "4/4";
        
        while (current_line < lines.size()) {
            if (lines[current_line].empty()) {
                current_line++;
                continue;
            }
            
            std::string line = lines[current_line];
            
            if (startsWith(line, "Title:")) metadata.title = extractValue(line);
            else if (startsWith(line, "Composer:")) metadata.composer = extractValue(line);
            else if (startsWith(line, "Key:")) metadata.key = extractValue(line);
            else if (startsWith(line, "Tempo:")) {
                try { metadata.tempo = std::stoi(extractValue(line)); } catch(...) {}
            }
            else if (startsWith(line, "TimeSignature:")) metadata.time_signature = extractValue(line);
            else if (startsWith(line, "Difficulty:")) {
                try { metadata.difficulty = std::stoi(extractValue(line)); } catch(...) {}
            }
            else if (startsWith(line, "Map {")) break;
            
            current_line++;
        }
    }

    bool parseMap() {
        if (current_line >= lines.size() || !startsWith(lines[current_line], "Map {")) {
            return false;
        }
        
        map_block.defined = true;
        current_line++;
        
        while (current_line < lines.size()) {
            if (lines[current_line].empty()) {
                current_line++;
                continue;
            }
            
            std::string line = trim(lines[current_line]);
            if (line == "}") {
                current_line++;
                return true;
            }
            
            if (startsWith(line, "Key:")) map_block.key = extractValue(line);
            else if (startsWith(line, "Scale:")) map_block.scale = extractValue(line);
            
            current_line++;
        }
        
        return false;
    }

    void generateNoteMapping() {
        std::map<std::string, std::vector<std::string>> scale_mappings = {
            {"C_Major", {"C", "D", "E", "F", "G", "A", "B"}},
            {"D_Major", {"D", "E", "F#", "G", "A", "B", "C#"}},
            {"G_Major", {"G", "A", "B", "C", "D", "E", "F#"}},
            {"A_Minor", {"A", "B", "C", "D", "E", "F", "G"}},
        };
        
        std::string key = map_block.key + "_" + map_block.scale;
        std::vector<std::string> scale = scale_mappings.count(key) ? 
            scale_mappings[key] : scale_mappings["C_Major"];
        
        for (int i = 0; i < 7; i++) {
            map_block.note_mapping[i + 1] = scale[i];
        }
    }

    void parseSegment() {
        std::regex seg_regex(R"(Segment\((\d+),\s*([A-Z_]+)\))");
        std::smatch match;
        
        if (std::regex_search(lines[current_line], match, seg_regex)) {
            Segment seg;
            seg.id = std::stoi(match[1]);
            seg.name = match[2];
            seg.tempo = metadata.tempo;
            seg.definition_line = current_line;
            
            current_line++;
            
            while (current_line < lines.size()) {
                if (lines[current_line].empty()) {
                    current_line++;
                    continue;
                }
                
                std::string line = trim(lines[current_line]);
                
                if (line == "END;") {
                    segments.push_back(seg);
                    current_line++;
                    break;
                }
                
                if (startsWith(line, "Tempo(")) {
                    seg.tempo = extractNumber(line);
                } else if (startsWith(line, "Begin.LEFT {")) {
                    seg.left = parseHand();
                } else if (startsWith(line, "Begin.RIGHT {")) {
                    seg.right = parseHand();
                }
                
                current_line++;
            }
        }
    }

    Hand parseHand() {
        Hand hand;
        current_line++;
        
        std::string chunk_data;
        while (current_line < lines.size()) {
            if (lines[current_line].empty()) {
                current_line++;
                continue;
            }
            
            std::string line = trim(lines[current_line]);
            
            if (line == "}") {
                if (!chunk_data.empty()) {
                    hand.chunks = parseChunks(chunk_data);
                }
                break;
            }
            
            if (!startsWith(line, "SYNC()") && !startsWith(line, "Position(")) {
                chunk_data += line + " ";
            }
            current_line++;
        }
        
        return hand;
    }

    std::vector<std::vector<Chord>> parseChunks(const std::string& data) {
        std::vector<std::vector<Chord>> result;
        std::vector<std::string> chunks = split(data, '|');
        
        for (const auto& chunk_str : chunks) {
            if (chunk_str.empty()) continue;
            std::vector<Chord> chords = parseChordSequence(chunk_str);
            if (!chords.empty()) result.push_back(chords);
        }
        
        return result;
    }

    std::vector<Chord> parseChordSequence(const std::string& str) {
        std::vector<Chord> chords;
        std::vector<std::string> parts = split(str, ',');
        
        for (const auto& part : parts) {
            std::string trimmed = trim(part);
            if (!trimmed.empty()) {
                chords.push_back(parseChord(trimmed));
            }
        }
        
        return chords;
    }

    Chord parseChord(const std::string& str) {
        Chord chord;
        
        bool is_chord = false;
        for (size_t i = 0; i < str.size() - 1; i++) {
            if (str[i] == '.' && i > 0 && i + 1 < str.size() && 
                std::isdigit(str[i-1]) && std::isdigit(str[i+1])) {
                is_chord = true;
                break;
            }
        }
        
        if (is_chord) {
            std::vector<std::string> note_parts;
            std::string current;
            
            for (size_t i = 0; i < str.size(); i++) {
                if (str[i] == '.' && i + 1 < str.size() && std::isdigit(str[i + 1])) {
                    if (!current.empty()) note_parts.push_back(current);
                    current.clear();
                } else {
                    current += str[i];
                }
            }
            if (!current.empty()) note_parts.push_back(current);
            
            if (!note_parts.empty()) {
                std::string last = note_parts.back();
                note_parts.pop_back();
                
                Note duration_note = parseNote(last);
                chord.duration = duration_note.duration;
                chord.is_dotted = duration_note.is_dotted;
                
                if (duration_note.degree > 0) {
                    note_parts.push_back(std::to_string(duration_note.degree));
                }
                
                for (const auto& np : note_parts) {
                    if (!np.empty() && std::isdigit(np[0])) {
                        Note n;
                        n.degree = std::stoi(np);
                        n.duration = chord.duration;
                        n.is_dotted = chord.is_dotted;
                        chord.notes.push_back(n);
                    }
                }
            }
        } else {
            Note n = parseNote(str);
            chord.notes.push_back(n);
            chord.duration = n.duration;
            chord.is_dotted = n.is_dotted;
        }
        
        return chord;
    }

    Note parseNote(const std::string& str) {
        Note note;
        if (str.empty()) return note;
        
        if (str[0] == 'R') {
            note.is_rest = true;
            parseDuration(str.substr(1), note);
            return note;
        }
        
        size_t i = 0;
        std::string degree_str;
        while (i < str.size() && std::isdigit(str[i])) {
            degree_str += str[i++];
        }
        
        if (!degree_str.empty()) {
            note.degree = std::stoi(degree_str);
        }
        
        while (i < str.size()) {
            if (str[i] == '#') {
                note.accidental = "#";
                i++;
            } else if (str[i] == 'b') {
                note.accidental = "b";
                i++;
            } else if (str[i] == '^') {
                i++;
                std::string shift_str;
                if (i < str.size() && str[i] == '-') {
                    shift_str += '-';
                    i++;
                }
                while (i < str.size() && std::isdigit(str[i])) {
                    shift_str += str[i++];
                }
                if (!shift_str.empty()) {
                    note.octave_shift = std::stoi(shift_str);
                }
            } else if (str[i] == 'p' || str[i] == 'f' || str[i] == 'm') {
                while (i < str.size() && (str[i] == 'p' || str[i] == 'f' || str[i] == 'm')) {
                    note.dynamic += str[i++];
                }
            } else if (str[i] == '.') {
                parseDuration(str.substr(i), note);
                break;
            } else {
                i++;
            }
        }
        
        return note;
    }

    void parseDuration(const std::string& str, Note& note) {
        if (str.empty()) {
            note.duration = 1.0;
        } else if (str == ".h") {
            note.duration = 2.0;
        } else if (str == ".w") {
            note.duration = 4.0;
        } else if (str == ".e") {
            note.duration = 0.5;
        } else if (str == ".s") {
            note.duration = 0.25;
        } else if (str == ".h.") {
            note.duration = 3.0;
            note.is_dotted = true;
        } else if (str == ".") {
            note.duration = 1.5;
            note.is_dotted = true;
        } else if (str == ".e.") {
            note.duration = 0.75;
            note.is_dotted = true;
        } else {
            note.duration = 1.0;
        }
    }

    std::string extractValue(const std::string& line) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string value = trim(line.substr(pos + 1));
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                return value.substr(1, value.size() - 2);
            }
            return value;
        }
        return "";
    }

    int extractNumber(const std::string& line) {
        std::regex num_regex(R"(\d+)");
        std::smatch match;
        if (std::regex_search(line, match, num_regex)) {
            return std::stoi(match[0]);
        }
        return 0;
    }
};

// ============================================
// MIDI Generator
// ============================================
class MIDIGenerator {
private:
    AMSParser& parser;
    MIDINoteConverter converter;
    int ticks_per_quarter = 480;
    
    int calculateTicks(double beats) {
        return static_cast<int>(beats * ticks_per_quarter);
    }
    
    uint32_t tempoToMicroseconds(int bpm) {
        return 60000000 / bpm;
    }

public:
    MIDIGenerator(AMSParser& p) : parser(p) {}
    
    bool generate(const std::string& filename) {
        const auto& metadata = parser.getMetadata();
        const auto& map_block = parser.getMapBlock();
        const auto& segments = parser.getSegments();
        
        MIDIWriter midi;
        
        midi.writeHeader(1, 3, ticks_per_quarter);
        
        // Track 0: Meta
        midi.startTrack();
        size_t track0_len_pos = midi.getTrackLengthPosition();
        size_t track0_start = midi.size();
        
        midi.writeDeltaTime(0);
        midi.writeTrackName(metadata.title);
        midi.writeDeltaTime(0);
        midi.writeTempoChange(tempoToMicroseconds(metadata.tempo));
        midi.writeDeltaTime(0);
        
        auto ts_parts = split(metadata.time_signature, '/');
        uint8_t numerator = 4, denominator = 4;
        if (ts_parts.size() == 2) {
            try {
                numerator = std::stoi(ts_parts[0]);
                denominator = std::stoi(ts_parts[1]);
            } catch(...) {}
        }
        midi.writeTimeSignature(numerator, denominator);
        
        midi.writeDeltaTime(0);
        midi.writeEndOfTrack();
        midi.updateTrackLength(track0_len_pos, midi.size() - track0_start);
        
        // Track 1: Left
        midi.startTrack();
        size_t track1_len_pos = midi.getTrackLengthPosition();
        size_t track1_start = midi.size();
        
        midi.writeDeltaTime(0);
        midi.writeTrackName("Left Hand");
        midi.writeDeltaTime(0);
        midi.writeProgramChange(0, 0);
        
        generateHandTrack(midi, segments, map_block, true, 0, 3);
        
        midi.writeDeltaTime(0);
        midi.writeEndOfTrack();
        midi.updateTrackLength(track1_len_pos, midi.size() - track1_start);
        
        // Track 2: Right
        midi.startTrack();
        size_t track2_len_pos = midi.getTrackLengthPosition();
        size_t track2_start = midi.size();
        
        midi.writeDeltaTime(0);
        midi.writeTrackName("Right Hand");
        midi.writeDeltaTime(0);
        midi.writeProgramChange(1, 0);
        
        generateHandTrack(midi, segments, map_block, false, 1, 4);
        
        midi.writeDeltaTime(0);
        midi.writeEndOfTrack();
        midi.updateTrackLength(track2_len_pos, midi.size() - track2_start);
        
        return midi.writeToFile(filename);
    }

private:
    void generateHandTrack(MIDIWriter& midi, const std::vector<Segment>& segments,
                          const MapBlock& map_block, bool is_left, uint8_t channel, int default_octave) {
        for (const auto& segment : segments) {
            const Hand& hand = is_left ? segment.left : segment.right;
            
            for (const auto& chunk : hand.chunks) {
                for (const auto& chord : chunk) {
                    if (chord.notes.empty()) continue;
                    
                    int duration_ticks = calculateTicks(chord.duration);
                    
                    if (!chord.notes.empty() && chord.notes[0].articulation == "!") {
                        duration_ticks = duration_ticks / 2;
                    }
                    
                    bool first_note = true;
                    for (const auto& note : chord.notes) {
                        if (note.is_rest || note.degree == 0) continue;
                        
                        std::string pitch = map_block.note_mapping.at(note.degree);
                        int midi_note = converter.pitchToMIDI(pitch, default_octave, note.octave_shift);
                        int velocity = converter.velocityFromDynamic(note.dynamic);
                        
                        if (note.articulation == "!") velocity = std::min(127, velocity + 20);
                        if (note.articulation == "~") velocity = std::max(40, velocity - 10);
                        if (note.articulation == ">") velocity = std::min(127, velocity + 30);
                        
                        midi.writeDeltaTime(first_note ? 0 : 0);
                        midi.writeNoteOn(channel, midi_note, velocity);
                        first_note = false;
                    }
                    
                    first_note = true;
                    for (const auto& note : chord.notes) {
                        if (note.is_rest || note.degree == 0) continue;
                        
                        std::string pitch = map_block.note_mapping.at(note.degree);
                        int midi_note = converter.pitchToMIDI(pitch, default_octave, note.octave_shift);
                        
                        midi.writeDeltaTime(first_note ? duration_ticks : 0);
                        midi.writeNoteOff(channel, midi_note);
                        first_note = false;
                    }
                }
            }
        }
    }
};

// ============================================
// Main
// ============================================
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.ams>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];

    std::cout << "\n╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║           AMS to MIDI Converter v3.0-Beta                     ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n\n";
    std::cout << "Processing: " << filename << "\n\n";

    AMSParser parser(filename);

    if (!parser.parse()) {
        std::cerr << "✗ Parsing failed!\n";
        parser.printErrors();
        return 1;
    }

    std::cout << "✓ AMS file parsed successfully\n";
    
    const auto& metadata = parser.getMetadata();
    std::cout << "  Title: " << metadata.title << "\n";
    std::cout << "  Composer: " << metadata.composer << "\n";
    std::cout << "  Key: " << metadata.key << " " << parser.getMapBlock().scale << "\n";
    std::cout << "  Tempo: " << metadata.tempo << " BPM\n";
    std::cout << "  Segments: " << parser.getSegments().size() << "\n\n";
    
    std::cout << "Generating MIDI file...\n";
    MIDIGenerator generator(parser);
    
    std::string output_filename = replaceExtension(filename, ".mid");
    
    if (generator.generate(output_filename)) {
        std::cout << "\n✓ MIDI file generated successfully!\n";
        std::cout << "  Output: " << output_filename << "\n\n";
        
        std::cout << "🎵 MIDI Details:\n";
        std::cout << "  Format: MIDI Format 1 (Multi-track)\n";
        std::cout << "  Tracks: 3 (Meta + Left Hand + Right Hand)\n";
        std::cout << "  Resolution: 480 ticks per quarter note\n";
        std::cout << "  Left Hand: Channel 0 (Acoustic Grand Piano)\n";
        std::cout << "  Right Hand: Channel 1 (Acoustic Grand Piano)\n\n";
        
        std::cout << "💡 You can now:\n";
        std::cout << "  - Open " << output_filename << " in any DAW (FL Studio, Ableton, etc.)\n";
        std::cout << "  - Import into MuseScore or other notation software\n";
        std::cout << "  - Play with VLC, Windows Media Player, or any MIDI player\n";
        std::cout << "  - Edit with a MIDI editor\n\n";
        
    } else {
        std::cerr << "\n✗ Failed to write MIDI file: " << output_filename << "\n";
        return 1;
    }

    return 0;
}
