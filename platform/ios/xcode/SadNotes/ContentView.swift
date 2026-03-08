// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  تطبيق الملاحظات — واجهة المستخدم الرئيسية
// ═══════════════════════════════════════════════════════════════════════════════

import SwiftUI

// ═══════════════════════════════════════════════════════════════════════════════
//  الواجهة الرئيسية
// ═══════════════════════════════════════════════════════════════════════════════

struct ContentView: View {
    @EnvironmentObject var store: NotesStore
    @State private var selectedTab = 0
    @State private var searchText = ""
    @State private var showingNewNote = false
    @State private var selectedNote: Note?
    @AppStorage("isDarkMode") private var isDarkMode = false
    
    var body: some View {
        TabView(selection: $selectedTab) {
            // تبويب جميع الملاحظات
            NavigationStack {
                NotesGridView(
                    notes: filteredNotes,
                    showingNewNote: $showingNewNote,
                    selectedNote: $selectedNote
                )
                .navigationTitle("ملاحظاتي")
                .searchable(text: $searchText, prompt: "ابحث في الملاحظات...")
                .toolbar {
                    ToolbarItem(placement: .navigationBarLeading) {
                        Button {
                            isDarkMode.toggle()
                        } label: {
                            Image(systemName: isDarkMode ? "sun.max.fill" : "moon.fill")
                        }
                    }
                    ToolbarItem(placement: .navigationBarTrailing) {
                        Button {
                            showingNewNote = true
                        } label: {
                            Image(systemName: "plus")
                        }
                    }
                }
            }
            .tabItem {
                Label("الكل", systemImage: "note.text")
            }
            .tag(0)
            
            // تبويب المثبتة
            NavigationStack {
                NotesGridView(
                    notes: store.pinnedNotes,
                    showingNewNote: $showingNewNote,
                    selectedNote: $selectedNote
                )
                .navigationTitle("المثبتة")
            }
            .tabItem {
                Label("مثبتة", systemImage: "pin.fill")
            }
            .tag(1)
            
            // تبويب المحذوفة
            NavigationStack {
                TrashView()
            }
            .tabItem {
                Label("محذوفة", systemImage: "trash.fill")
            }
            .tag(2)
        }
        .sheet(isPresented: $showingNewNote) {
            NoteEditorView(note: nil)
        }
        .sheet(item: $selectedNote) { note in
            NoteEditorView(note: note)
        }
    }
    
    var filteredNotes: [Note] {
        if searchText.isEmpty {
            return store.activeNotes
        }
        return store.search(searchText)
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  شبكة الملاحظات
// ═══════════════════════════════════════════════════════════════════════════════

struct NotesGridView: View {
    let notes: [Note]
    @Binding var showingNewNote: Bool
    @Binding var selectedNote: Note?
    
    let columns = [
        GridItem(.flexible()),
        GridItem(.flexible())
    ]
    
    var body: some View {
        if notes.isEmpty {
            EmptyStateView(
                icon: "note.text",
                message: "لا توجد ملاحظات\nاضغط + لإضافة ملاحظة جديدة"
            )
        } else {
            ScrollView {
                LazyVGrid(columns: columns, spacing: 12) {
                    ForEach(notes) { note in
                        NoteCardView(note: note)
                            .onTapGesture {
                                selectedNote = note
                            }
                    }
                }
                .padding()
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  بطاقة الملاحظة
// ═══════════════════════════════════════════════════════════════════════════════

struct NoteCardView: View {
    let note: Note
    @EnvironmentObject var store: NotesStore
    @State private var showingActions = false
    
    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            // العنوان مع أيقونة التثبيت
            HStack {
                Text(note.title)
                    .font(.headline)
                    .lineLimit(1)
                
                Spacer()
                
                if note.isPinned {
                    Image(systemName: "pin.fill")
                        .font(.caption)
                        .foregroundStyle(.orange)
                }
            }
            
            // المحتوى
            Text(note.content)
                .font(.subheadline)
                .foregroundStyle(.secondary)
                .lineLimit(4)
            
            Spacer()
            
            // التاريخ
            Text(note.modifiedAt, style: .date)
                .font(.caption2)
                .foregroundStyle(.tertiary)
        }
        .padding()
        .frame(height: 150)
        .background(note.color)
        .clipShape(RoundedRectangle(cornerRadius: 12))
        .shadow(color: .black.opacity(0.1), radius: 4, x: 0, y: 2)
        .contextMenu {
            Button {
                store.togglePin(note)
            } label: {
                Label(
                    note.isPinned ? "إلغاء التثبيت" : "تثبيت",
                    systemImage: note.isPinned ? "pin.slash" : "pin"
                )
            }
            
            Button {
                showingActions = true
            } label: {
                Label("تغيير اللون", systemImage: "paintpalette")
            }
            
            Button {
                shareNote()
            } label: {
                Label("مشاركة", systemImage: "square.and.arrow.up")
            }
            
            Divider()
            
            Button(role: .destructive) {
                store.delete(note)
            } label: {
                Label("حذف", systemImage: "trash")
            }
        }
        .sheet(isPresented: $showingActions) {
            ColorPickerSheet(note: note)
        }
    }
    
    func shareNote() {
        let text = "\(note.title)\n\n\(note.content)"
        let activityVC = UIActivityViewController(
            activityItems: [text],
            applicationActivities: nil
        )
        
        if let windowScene = UIApplication.shared.connectedScenes.first as? UIWindowScene,
           let window = windowScene.windows.first,
           let rootVC = window.rootViewController {
            rootVC.present(activityVC, animated: true)
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  منتقي الألوان
// ═══════════════════════════════════════════════════════════════════════════════

struct ColorPickerSheet: View {
    let note: Note
    @EnvironmentObject var store: NotesStore
    @Environment(\.dismiss) var dismiss
    
    let colors: [(name: String, hex: String)] = [
        ("أصفر", "#FFFDE7"),
        ("برتقالي", "#FFF3E0"),
        ("أحمر", "#FFEBEE"),
        ("وردي", "#FCE4EC"),
        ("بنفسجي", "#F3E5F5"),
        ("أزرق", "#E3F2FD"),
        ("أزرق فاتح", "#E0F7FA"),
        ("أخضر", "#E8F5E9"),
        ("رمادي", "#FAFAFA")
    ]
    
    var body: some View {
        NavigationStack {
            VStack(spacing: 20) {
                Text("اختر لوناً")
                    .font(.headline)
                
                LazyVGrid(columns: [GridItem(.adaptive(minimum: 60))], spacing: 16) {
                    ForEach(colors, id: \.hex) { color in
                        Circle()
                            .fill(Color(hex: color.hex) ?? .gray)
                            .frame(width: 50, height: 50)
                            .overlay(
                                Circle()
                                    .stroke(Color.primary.opacity(0.3), lineWidth: 1)
                            )
                            .overlay {
                                if note.colorHex == color.hex {
                                    Image(systemName: "checkmark")
                                        .foregroundStyle(.primary)
                                }
                            }
                            .onTapGesture {
                                store.changeColor(note, to: color.hex)
                                dismiss()
                            }
                    }
                }
                .padding()
                
                Spacer()
            }
            .padding()
            .navigationTitle("تغيير اللون")
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .cancellationAction) {
                    Button("إلغاء") {
                        dismiss()
                    }
                }
            }
        }
        .presentationDetents([.medium])
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  محرر الملاحظة
// ═══════════════════════════════════════════════════════════════════════════════

struct NoteEditorView: View {
    let note: Note?
    @EnvironmentObject var store: NotesStore
    @Environment(\.dismiss) var dismiss
    
    @State private var title: String = ""
    @State private var content: String = ""
    @State private var hasChanges = false
    @State private var showingDiscardAlert = false
    
    var isNewNote: Bool { note == nil }
    
    var body: some View {
        NavigationStack {
            VStack(spacing: 0) {
                // حقل العنوان
                TextField("العنوان", text: $title)
                    .font(.title2.bold())
                    .padding()
                    .onChange(of: title) { _, _ in hasChanges = true }
                
                Divider()
                
                // حقل المحتوى
                TextEditor(text: $content)
                    .font(.body)
                    .padding(.horizontal)
                    .onChange(of: content) { _, _ in hasChanges = true }
            }
            .navigationTitle(isNewNote ? "ملاحظة جديدة" : "تحرير الملاحظة")
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .cancellationAction) {
                    Button("إلغاء") {
                        if hasChanges {
                            showingDiscardAlert = true
                        } else {
                            dismiss()
                        }
                    }
                }
                
                ToolbarItem(placement: .confirmationAction) {
                    Button("حفظ") {
                        saveNote()
                    }
                    .disabled(title.trimmingCharacters(in: .whitespaces).isEmpty)
                }
            }
            .alert("تجاهل التغييرات؟", isPresented: $showingDiscardAlert) {
                Button("تجاهل", role: .destructive) {
                    dismiss()
                }
                Button("إلغاء", role: .cancel) { }
            } message: {
                Text("لديك تغييرات غير محفوظة. هل تريد تجاهلها؟")
            }
            .onAppear {
                if let note = note {
                    title = note.title
                    content = note.content
                }
            }
        }
    }
    
    func saveNote() {
        if let existingNote = note {
            var updated = existingNote
            updated.title = title.trimmingCharacters(in: .whitespaces)
            updated.content = content
            store.update(updated)
        } else {
            let newNote = Note(
                title: title.trimmingCharacters(in: .whitespaces),
                content: content
            )
            store.add(newNote)
        }
        dismiss()
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  سلة المحذوفات
// ═══════════════════════════════════════════════════════════════════════════════

struct TrashView: View {
    @EnvironmentObject var store: NotesStore
    
    var body: some View {
        Group {
            if store.deletedNotes.isEmpty {
                EmptyStateView(
                    icon: "trash",
                    message: "سلة المحذوفات فارغة"
                )
            } else {
                List {
                    ForEach(store.deletedNotes) { note in
                        VStack(alignment: .leading) {
                            Text(note.title)
                                .font(.headline)
                            Text(note.content)
                                .font(.subheadline)
                                .foregroundStyle(.secondary)
                                .lineLimit(2)
                        }
                        .swipeActions(edge: .leading) {
                            Button {
                                store.restore(note)
                            } label: {
                                Label("استعادة", systemImage: "arrow.uturn.backward")
                            }
                            .tint(.green)
                        }
                        .swipeActions(edge: .trailing) {
                            Button(role: .destructive) {
                                store.permanentlyDelete(note)
                            } label: {
                                Label("حذف نهائي", systemImage: "trash.fill")
                            }
                        }
                    }
                }
            }
        }
        .navigationTitle("سلة المحذوفات")
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  حالة فارغة
// ═══════════════════════════════════════════════════════════════════════════════

struct EmptyStateView: View {
    let icon: String
    let message: String
    
    var body: some View {
        VStack(spacing: 16) {
            Image(systemName: icon)
                .font(.system(size: 60))
                .foregroundStyle(.secondary)
            
            Text(message)
                .font(.body)
                .foregroundStyle(.secondary)
                .multilineTextAlignment(.center)
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  معاينة
// ═══════════════════════════════════════════════════════════════════════════════

#Preview {
    ContentView()
        .environmentObject(NotesStore())
}
