// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadContactsBridge.swift
// الوصف: جسر جهات الاتصال — iOS (Contacts Framework)
// ═══════════════════════════════════════════════════════════════════════════════

import Contacts

/// جهة اتصال مبسطة
public struct SadContact {
    public let identifier: String
    public let firstName: String
    public let lastName: String
    public let phoneNumbers: [String]
    public let emails: [String]
    
    public var fullName: String {
        "\(firstName) \(lastName)".trimmingCharacters(in: .whitespaces)
    }
}

/// جسر جهات الاتصال لغة ص — قراءة وإدارة جهات الاتصال
public final class SadContactsBridge {
    
    public static let shared = SadContactsBridge()
    private init() {}
    
    private let store = CNContactStore()
    
    // ═════════════════════════════════════════════════════════════════════
    // فحص الأذونات
    // ═════════════════════════════════════════════════════════════════════
    
    /// هل الوصول مسموح
    public var isAuthorized: Bool {
        CNContactStore.authorizationStatus(for: .contacts) == .authorized
    }
    
    /// طلب إذن الوصول
    public func requestAccess(completion: @escaping (Bool) -> Void) {
        store.requestAccess(for: .contacts) { granted, _ in
            DispatchQueue.main.async { completion(granted) }
        }
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // جلب جهات الاتصال
    // ═════════════════════════════════════════════════════════════════════
    
    /// جلب كل جهات الاتصال
    public func fetchAll(completion: @escaping ([SadContact]) -> Void) {
        DispatchQueue.global().async { [weak self] in
            guard let self = self else { return }
            let keysToFetch: [CNKeyDescriptor] = [
                CNContactGivenNameKey as CNKeyDescriptor,
                CNContactFamilyNameKey as CNKeyDescriptor,
                CNContactPhoneNumbersKey as CNKeyDescriptor,
                CNContactEmailAddressesKey as CNKeyDescriptor
            ]
            
            var contacts: [SadContact] = []
            let request = CNContactFetchRequest(keysToFetch: keysToFetch)
            request.sortOrder = .givenName
            
            do {
                try self.store.enumerateContacts(with: request) { cnContact, _ in
                    let contact = self.mapContact(cnContact)
                    contacts.append(contact)
                }
            } catch {
                print("خطأ في جلب جهات الاتصال: \(error)")
            }
            
            DispatchQueue.main.async { completion(contacts) }
        }
    }
    
    /// البحث عن جهات اتصال بالاسم
    public func search(name: String, completion: @escaping ([SadContact]) -> Void) {
        DispatchQueue.global().async { [weak self] in
            guard let self = self else { return }
            let keysToFetch: [CNKeyDescriptor] = [
                CNContactGivenNameKey as CNKeyDescriptor,
                CNContactFamilyNameKey as CNKeyDescriptor,
                CNContactPhoneNumbersKey as CNKeyDescriptor,
                CNContactEmailAddressesKey as CNKeyDescriptor
            ]
            
            let predicate = CNContact.predicateForContacts(matchingName: name)
            let results = (try? self.store.unifiedContacts(matching: predicate,
                                                           keysToFetch: keysToFetch)) ?? []
            let contacts = results.map { self.mapContact($0) }
            DispatchQueue.main.async { completion(contacts) }
        }
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // إضافة جهة اتصال
    // ═════════════════════════════════════════════════════════════════════
    
    /// إضافة جهة اتصال جديدة
    public func addContact(firstName: String, lastName: String,
                            phone: String? = nil, email: String? = nil) -> Bool {
        let contact = CNMutableContact()
        contact.givenName = firstName
        contact.familyName = lastName
        
        if let phone = phone {
            contact.phoneNumbers = [CNLabeledValue(label: CNLabelPhoneNumberMain,
                                                    value: CNPhoneNumber(stringValue: phone))]
        }
        if let email = email {
            contact.emailAddresses = [CNLabeledValue(label: CNLabelHome,
                                                      value: email as NSString)]
        }
        
        let saveRequest = CNSaveRequest()
        saveRequest.add(contact, toContainerWithIdentifier: nil)
        
        do {
            try store.execute(saveRequest)
            return true
        } catch {
            print("خطأ في إضافة جهة الاتصال: \(error)")
            return false
        }
    }
    
    /// حذف جهة اتصال
    public func deleteContact(identifier: String) -> Bool {
        let keysToFetch: [CNKeyDescriptor] = [CNContactIdentifierKey as CNKeyDescriptor]
        guard let contact = try? store.unifiedContact(withIdentifier: identifier,
                                                       keysToFetch: keysToFetch) else {
            return false
        }
        
        let mutableContact = contact.mutableCopy() as! CNMutableContact
        let saveRequest = CNSaveRequest()
        saveRequest.delete(mutableContact)
        
        do {
            try store.execute(saveRequest)
            return true
        } catch {
            print("خطأ في حذف جهة الاتصال: \(error)")
            return false
        }
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // العدد
    // ═════════════════════════════════════════════════════════════════════
    
    /// عدد جهات الاتصال
    public func count() -> Int {
        let keysToFetch: [CNKeyDescriptor] = [CNContactIdentifierKey as CNKeyDescriptor]
        let request = CNContactFetchRequest(keysToFetch: keysToFetch)
        var count = 0
        try? store.enumerateContacts(with: request) { _, _ in count += 1 }
        return count
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // مساعدات
    // ═════════════════════════════════════════════════════════════════════
    
    private func mapContact(_ cn: CNContact) -> SadContact {
        SadContact(
            identifier: cn.identifier,
            firstName: cn.givenName,
            lastName: cn.familyName,
            phoneNumbers: cn.phoneNumbers.map { $0.value.stringValue },
            emails: cn.emailAddresses.map { $0.value as String }
        )
    }
}
