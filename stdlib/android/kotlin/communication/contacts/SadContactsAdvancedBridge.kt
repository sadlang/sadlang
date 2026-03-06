package sad.android.communication.contacts

import android.Manifest
import android.content.ContentResolver
import android.content.ContentUris
import android.content.ContentValues
import android.content.Context
import android.content.pm.PackageManager
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.net.Uri
import android.provider.ContactsContract
import androidx.core.content.ContextCompat
import java.io.ByteArrayOutputStream

/**
 * جسر جهات الاتصال المتقدم
 * Advanced Contacts Bridge - إدارة جهات الاتصال
 * 
 * @author فريق لغة ص
 */
object SadContactsAdvancedBridge {
    
    /**
     * جهة اتصال كاملة / Full contact
     */
    data class Contact(
        val id: Long = 0,
        val displayName: String,
        val givenName: String = "",
        val familyName: String = "",
        val phones: List<Phone> = emptyList(),
        val emails: List<Email> = emptyList(),
        val addresses: List<Address> = emptyList(),
        val organization: String = "",
        val jobTitle: String = "",
        val note: String = "",
        val photoUri: Uri? = null,
        val birthday: String = "",
        val isFavorite: Boolean = false
    )
    
    data class Phone(val number: String, val type: PhoneType = PhoneType.MOBILE)
    data class Email(val address: String, val type: EmailType = EmailType.HOME)
    data class Address(val street: String, val city: String = "", val country: String = "")
    
    enum class PhoneType { MOBILE, HOME, WORK, FAX, OTHER }
    enum class EmailType { HOME, WORK, OTHER }
    
    // ═══════════════════════════════════════════════════════════════
    //                         قراءة جهات الاتصال
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * جلب كل جهات الاتصال / Get all contacts
     */
    fun getAll(context: Context): List<Contact> {
        if (!hasPermission(context)) return emptyList()
        
        val contacts = mutableListOf<Contact>()
        val resolver = context.contentResolver
        
        val cursor = resolver.query(
            ContactsContract.Contacts.CONTENT_URI,
            null, null, null,
            ContactsContract.Contacts.DISPLAY_NAME + " ASC"
        )
        
        cursor?.use {
            while (it.moveToNext()) {
                val id = it.getLong(it.getColumnIndexOrThrow(ContactsContract.Contacts._ID))
                val name = it.getString(it.getColumnIndexOrThrow(ContactsContract.Contacts.DISPLAY_NAME)) ?: ""
                val starred = it.getInt(it.getColumnIndexOrThrow(ContactsContract.Contacts.STARRED)) == 1
                
                contacts.add(Contact(
                    id = id,
                    displayName = name,
                    phones = getPhones(resolver, id),
                    emails = getEmails(resolver, id),
                    isFavorite = starred
                ))
            }
        }
        
        return contacts
    }
    
    /**
     * البحث في جهات الاتصال / Search contacts
     */
    fun search(context: Context, query: String): List<Contact> {
        if (!hasPermission(context)) return emptyList()
        
        val contacts = mutableListOf<Contact>()
        val resolver = context.contentResolver
        
        val cursor = resolver.query(
            ContactsContract.Contacts.CONTENT_URI,
            null,
            "${ContactsContract.Contacts.DISPLAY_NAME} LIKE ?",
            arrayOf("%$query%"),
            ContactsContract.Contacts.DISPLAY_NAME + " ASC"
        )
        
        cursor?.use {
            while (it.moveToNext()) {
                val id = it.getLong(it.getColumnIndexOrThrow(ContactsContract.Contacts._ID))
                val name = it.getString(it.getColumnIndexOrThrow(ContactsContract.Contacts.DISPLAY_NAME)) ?: ""
                
                contacts.add(Contact(
                    id = id,
                    displayName = name,
                    phones = getPhones(resolver, id)
                ))
            }
        }
        
        return contacts
    }
    
    /**
     * جلب جهة اتصال بالمعرف / Get contact by ID
     */
    fun getById(context: Context, contactId: Long): Contact? {
        if (!hasPermission(context)) return null
        
        val resolver = context.contentResolver
        val uri = ContentUris.withAppendedId(ContactsContract.Contacts.CONTENT_URI, contactId)
        
        val cursor = resolver.query(uri, null, null, null, null)
        
        cursor?.use {
            if (it.moveToFirst()) {
                val name = it.getString(it.getColumnIndexOrThrow(ContactsContract.Contacts.DISPLAY_NAME)) ?: ""
                
                return Contact(
                    id = contactId,
                    displayName = name,
                    phones = getPhones(resolver, contactId),
                    emails = getEmails(resolver, contactId),
                    addresses = getAddresses(resolver, contactId)
                )
            }
        }
        
        return null
    }
    
    /**
     * جلب المفضلة / Get favorites
     */
    fun getFavorites(context: Context): List<Contact> {
        return getAll(context).filter { it.isFavorite }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إضافة جهات اتصال
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إضافة جهة اتصال جديدة / Add new contact
     */
    fun add(context: Context, contact: Contact): Long? {
        if (!hasWritePermission(context)) return null
        
        val resolver = context.contentResolver
        
        // إضافة جهة اتصال فارغة
        val rawContactValues = ContentValues()
        val rawContactUri = resolver.insert(
            ContactsContract.RawContacts.CONTENT_URI,
            rawContactValues
        ) ?: return null
        
        val rawContactId = ContentUris.parseId(rawContactUri)
        
        // إضافة الاسم
        val nameValues = ContentValues().apply {
            put(ContactsContract.Data.RAW_CONTACT_ID, rawContactId)
            put(ContactsContract.Data.MIMETYPE, ContactsContract.CommonDataKinds.StructuredName.CONTENT_ITEM_TYPE)
            put(ContactsContract.CommonDataKinds.StructuredName.DISPLAY_NAME, contact.displayName)
            put(ContactsContract.CommonDataKinds.StructuredName.GIVEN_NAME, contact.givenName)
            put(ContactsContract.CommonDataKinds.StructuredName.FAMILY_NAME, contact.familyName)
        }
        resolver.insert(ContactsContract.Data.CONTENT_URI, nameValues)
        
        // إضافة الهواتف
        contact.phones.forEach { phone ->
            val phoneValues = ContentValues().apply {
                put(ContactsContract.Data.RAW_CONTACT_ID, rawContactId)
                put(ContactsContract.Data.MIMETYPE, ContactsContract.CommonDataKinds.Phone.CONTENT_ITEM_TYPE)
                put(ContactsContract.CommonDataKinds.Phone.NUMBER, phone.number)
                put(ContactsContract.CommonDataKinds.Phone.TYPE, phoneTypeToInt(phone.type))
            }
            resolver.insert(ContactsContract.Data.CONTENT_URI, phoneValues)
        }
        
        // إضافة البريد
        contact.emails.forEach { email ->
            val emailValues = ContentValues().apply {
                put(ContactsContract.Data.RAW_CONTACT_ID, rawContactId)
                put(ContactsContract.Data.MIMETYPE, ContactsContract.CommonDataKinds.Email.CONTENT_ITEM_TYPE)
                put(ContactsContract.CommonDataKinds.Email.ADDRESS, email.address)
                put(ContactsContract.CommonDataKinds.Email.TYPE, emailTypeToInt(email.type))
            }
            resolver.insert(ContactsContract.Data.CONTENT_URI, emailValues)
        }
        
        return rawContactId
    }
    
    /**
     * إضافة سريعة / Quick add
     */
    fun quickAdd(context: Context, name: String, phone: String): Long? {
        return add(context, Contact(
            displayName = name,
            phones = listOf(Phone(phone))
        ))
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         حذف وتعديل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * حذف جهة اتصال / Delete contact
     */
    fun delete(context: Context, contactId: Long): Boolean {
        if (!hasWritePermission(context)) return false
        
        val uri = ContentUris.withAppendedId(ContactsContract.Contacts.CONTENT_URI, contactId)
        return context.contentResolver.delete(uri, null, null) > 0
    }
    
    /**
     * تعيين كمفضل / Set as favorite
     */
    fun setFavorite(context: Context, contactId: Long, favorite: Boolean): Boolean {
        if (!hasWritePermission(context)) return false
        
        val values = ContentValues().apply {
            put(ContactsContract.Contacts.STARRED, if (favorite) 1 else 0)
        }
        
        val uri = ContentUris.withAppendedId(ContactsContract.Contacts.CONTENT_URI, contactId)
        return context.contentResolver.update(uri, values, null, null) > 0
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun getPhones(resolver: ContentResolver, contactId: Long): List<Phone> {
        val phones = mutableListOf<Phone>()
        
        val cursor = resolver.query(
            ContactsContract.CommonDataKinds.Phone.CONTENT_URI,
            null,
            ContactsContract.CommonDataKinds.Phone.CONTACT_ID + " = ?",
            arrayOf(contactId.toString()),
            null
        )
        
        cursor?.use {
            while (it.moveToNext()) {
                val number = it.getString(it.getColumnIndexOrThrow(ContactsContract.CommonDataKinds.Phone.NUMBER))
                phones.add(Phone(number))
            }
        }
        
        return phones
    }
    
    private fun getEmails(resolver: ContentResolver, contactId: Long): List<Email> {
        val emails = mutableListOf<Email>()
        
        val cursor = resolver.query(
            ContactsContract.CommonDataKinds.Email.CONTENT_URI,
            null,
            ContactsContract.CommonDataKinds.Email.CONTACT_ID + " = ?",
            arrayOf(contactId.toString()),
            null
        )
        
        cursor?.use {
            while (it.moveToNext()) {
                val address = it.getString(it.getColumnIndexOrThrow(ContactsContract.CommonDataKinds.Email.ADDRESS))
                emails.add(Email(address))
            }
        }
        
        return emails
    }
    
    private fun getAddresses(resolver: ContentResolver, contactId: Long): List<Address> {
        val addresses = mutableListOf<Address>()
        
        val cursor = resolver.query(
            ContactsContract.CommonDataKinds.StructuredPostal.CONTENT_URI,
            null,
            ContactsContract.CommonDataKinds.StructuredPostal.CONTACT_ID + " = ?",
            arrayOf(contactId.toString()),
            null
        )
        
        cursor?.use {
            while (it.moveToNext()) {
                val street = it.getString(it.getColumnIndexOrThrow(ContactsContract.CommonDataKinds.StructuredPostal.STREET)) ?: ""
                val city = it.getString(it.getColumnIndexOrThrow(ContactsContract.CommonDataKinds.StructuredPostal.CITY)) ?: ""
                val country = it.getString(it.getColumnIndexOrThrow(ContactsContract.CommonDataKinds.StructuredPostal.COUNTRY)) ?: ""
                addresses.add(Address(street, city, country))
            }
        }
        
        return addresses
    }
    
    private fun phoneTypeToInt(type: PhoneType): Int = when (type) {
        PhoneType.MOBILE -> ContactsContract.CommonDataKinds.Phone.TYPE_MOBILE
        PhoneType.HOME -> ContactsContract.CommonDataKinds.Phone.TYPE_HOME
        PhoneType.WORK -> ContactsContract.CommonDataKinds.Phone.TYPE_WORK
        PhoneType.FAX -> ContactsContract.CommonDataKinds.Phone.TYPE_FAX_HOME
        PhoneType.OTHER -> ContactsContract.CommonDataKinds.Phone.TYPE_OTHER
    }
    
    private fun emailTypeToInt(type: EmailType): Int = when (type) {
        EmailType.HOME -> ContactsContract.CommonDataKinds.Email.TYPE_HOME
        EmailType.WORK -> ContactsContract.CommonDataKinds.Email.TYPE_WORK
        EmailType.OTHER -> ContactsContract.CommonDataKinds.Email.TYPE_OTHER
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الأذونات
    // ═══════════════════════════════════════════════════════════════
    
    fun hasPermission(context: Context): Boolean {
        return ContextCompat.checkSelfPermission(
            context,
            Manifest.permission.READ_CONTACTS
        ) == PackageManager.PERMISSION_GRANTED
    }
    
    fun hasWritePermission(context: Context): Boolean {
        return ContextCompat.checkSelfPermission(
            context,
            Manifest.permission.WRITE_CONTACTS
        ) == PackageManager.PERMISSION_GRANTED
    }
    
    fun requiredPermissions(): Array<String> = arrayOf(
        Manifest.permission.READ_CONTACTS,
        Manifest.permission.WRITE_CONTACTS
    )
}
