// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  تطبيق الملاحظات — SadNotes iOS App
//  نقطة الدخول الرئيسية
// ═══════════════════════════════════════════════════════════════════════════════

import SwiftUI

@main
struct SadNotesApp: App {
    @StateObject private var notesStore = NotesStore()
    @AppStorage("isDarkMode") private var isDarkMode = false
    
    var body: some Scene {
        WindowGroup {
            ContentView()
                .environmentObject(notesStore)
                .preferredColorScheme(isDarkMode ? .dark : .light)
                .environment(\.layoutDirection, .rightToLeft)
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  نموذج الملاحظة
// ═══════════════════════════════════════════════════════════════════════════════

struct Note: Identifiable, Codable, Hashable {
    let id: UUID
    var title: String
    var content: String
    var colorHex: String
    var isPinned: Bool
    var isDeleted: Bool
    var createdAt: Date
    var modifiedAt: Date
    
    init(title: String, content: String, colorHex: String = "#FFFDE7") {
        self.id = UUID()
        self.title = title
        self.content = content
        self.colorHex = colorHex
        self.isPinned = false
        self.isDeleted = false
        self.createdAt = Date()
        self.modifiedAt = Date()
    }
    
    var color: Color {
        Color(hex: colorHex) ?? .yellow.opacity(0.3)
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  مخزن الملاحظات
// ═══════════════════════════════════════════════════════════════════════════════

class NotesStore: ObservableObject {
    @Published var notes: [Note] = []
    
    private let saveKey = "sad_notes_data"
    
    init() {
        loadNotes()
        
        // بيانات تجريبية إن لم توجد ملاحظات
        if notes.isEmpty {
            notes = [
                Note(title: "مرحباً بالعالم", content: "هذه أول ملاحظة في التطبيق!"),
                Note(title: "قائمة المهام", content: "- تعلم لغة ص\n- بناء تطبيقات\n- المشاركة مع المجتمع"),
                Note(title: "أفكار للمشروع", content: "• نظام إدارة المحتوى\n• تطبيق محادثة\n• لعبة تعليمية", colorHex: "#E3F2FD")
            ]
        }
    }
    
    var activeNotes: [Note] {
        notes.filter { !$0.isDeleted }
    }
    
    var pinnedNotes: [Note] {
        activeNotes.filter { $0.isPinned }
    }
    
    var unpinnedNotes: [Note] {
        activeNotes.filter { !$0.isPinned }
    }
    
    var deletedNotes: [Note] {
        notes.filter { $0.isDeleted }
    }
    
    func add(_ note: Note) {
        notes.insert(note, at: 0)
        saveNotes()
    }
    
    func update(_ note: Note) {
        if let index = notes.firstIndex(where: { $0.id == note.id }) {
            var updated = note
            updated.modifiedAt = Date()
            notes[index] = updated
            saveNotes()
        }
    }
    
    func delete(_ note: Note) {
        if let index = notes.firstIndex(where: { $0.id == note.id }) {
            notes[index].isDeleted = true
            saveNotes()
        }
    }
    
    func restore(_ note: Note) {
        if let index = notes.firstIndex(where: { $0.id == note.id }) {
            notes[index].isDeleted = false
            saveNotes()
        }
    }
    
    func permanentlyDelete(_ note: Note) {
        notes.removeAll { $0.id == note.id }
        saveNotes()
    }
    
    func togglePin(_ note: Note) {
        if let index = notes.firstIndex(where: { $0.id == note.id }) {
            notes[index].isPinned.toggle()
            saveNotes()
        }
    }
    
    func changeColor(_ note: Note, to colorHex: String) {
        if let index = notes.firstIndex(where: { $0.id == note.id }) {
            notes[index].colorHex = colorHex
            notes[index].modifiedAt = Date()
            saveNotes()
        }
    }
    
    func search(_ query: String) -> [Note] {
        guard !query.isEmpty else { return activeNotes }
        return activeNotes.filter {
            $0.title.localizedCaseInsensitiveContains(query) ||
            $0.content.localizedCaseInsensitiveContains(query)
        }
    }
    
    private func saveNotes() {
        if let encoded = try? JSONEncoder().encode(notes) {
            UserDefaults.standard.set(encoded, forKey: saveKey)
        }
    }
    
    private func loadNotes() {
        if let data = UserDefaults.standard.data(forKey: saveKey),
           let decoded = try? JSONDecoder().decode([Note].self, from: data) {
            notes = decoded
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  امتداد اللون
// ═══════════════════════════════════════════════════════════════════════════════

extension Color {
    init?(hex: String) {
        var hexSanitized = hex.trimmingCharacters(in: .whitespacesAndNewlines)
        hexSanitized = hexSanitized.replacingOccurrences(of: "#", with: "")
        
        var rgb: UInt64 = 0
        guard Scanner(string: hexSanitized).scanHexInt64(&rgb) else { return nil }
        
        let r = Double((rgb & 0xFF0000) >> 16) / 255.0
        let g = Double((rgb & 0x00FF00) >> 8) / 255.0
        let b = Double(rgb & 0x0000FF) / 255.0
        
        self.init(red: r, green: g, blue: b)
    }
}
