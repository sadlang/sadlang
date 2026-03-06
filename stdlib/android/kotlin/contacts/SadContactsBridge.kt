// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadContactsBridge.kt
// الوصف: جسر Kotlin لجهات الاتصال
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.ContentProviderOperation
import android.content.ContentResolver
import android.content.Context
import android.database.Cursor
import android.net.Uri
import android.provider.ContactsContract
import android.provider.ContactsContract.CommonDataKinds.*

/**
 * جسر جهات الاتصال — SadContactsBridge
 */
object SadContactsBridge {
    
    private var context: Context? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Query Contacts
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getAllContacts(): List<Map<String, Any?>> {
        val ctx = context ?: return emptyList()
        val contacts = mutableListOf<Map<String, Any?>>()
        
        val cursor = ctx.contentResolver.query(
            ContactsContract.Contacts.CONTENT_URI,
            null,
            null,
            null,
            ContactsContract.Contacts.DISPLAY_NAME + " ASC"
        )
        
        cursor?.use {
            val idIndex = it.getColumnIndex(ContactsContract.Contacts._ID)
            val nameIndex = it.getColumnIndex(ContactsContract.Contacts.DISPLAY_NAME)
            val hasPhoneIndex = it.getColumnIndex(ContactsContract.Contacts.HAS_PHONE_NUMBER)
            val photoUriIndex = it.getColumnIndex(ContactsContract.Contacts.PHOTO_URI)
            
            while (it.moveToNext()) {
                val id = it.getString(idIndex)
                val name = it.getString(nameIndex)
                val hasPhone = it.getInt(hasPhoneIndex) > 0
                val photoUri = it.getString(photoUriIndex)
                
                val phones = if (hasPhone) getPhoneNumbers(ctx, id) else emptyList()
                val emails = getEmails(ctx, id)
                
                contacts.add(mapOf(
                    "id" to id,
                    "name" to name,
                    "phones" to phones,
                    "emails" to emails,
                    "photoUri" to photoUri
                ))
            }
        }
        
        return contacts
    }
    
    @JvmStatic
    fun getContactById(contactId: String): Map<String, Any?>? {
        val ctx = context ?: return null
        
        val cursor = ctx.contentResolver.query(
            ContactsContract.Contacts.CONTENT_URI,
            null,
            ContactsContract.Contacts._ID + " = ?",
            arrayOf(contactId),
            null
        )
        
        cursor?.use {
            if (it.moveToFirst()) {
                val nameIndex = it.getColumnIndex(ContactsContract.Contacts.DISPLAY_NAME)
                val hasPhoneIndex = it.getColumnIndex(ContactsContract.Contacts.HAS_PHONE_NUMBER)
                val photoUriIndex = it.getColumnIndex(ContactsContract.Contacts.PHOTO_URI)
                
                val name = it.getString(nameIndex)
                val hasPhone = it.getInt(hasPhoneIndex) > 0
                val photoUri = it.getString(photoUriIndex)
                
                val phones = if (hasPhone) getPhoneNumbers(ctx, contactId) else emptyList()
                val emails = getEmails(ctx, contactId)
                val addresses = getAddresses(ctx, contactId)
                val organizations = getOrganizations(ctx, contactId)
                val websites = getWebsites(ctx, contactId)
                val notes = getNotes(ctx, contactId)
                
                return mapOf(
                    "id" to contactId,
                    "name" to name,
                    "phones" to phones,
                    "emails" to emails,
                    "addresses" to addresses,
                    "organizations" to organizations,
                    "websites" to websites,
                    "notes" to notes,
                    "photoUri" to photoUri
                )
            }
        }
        
        return null
    }
    
    @JvmStatic
    fun searchContacts(query: String): List<Map<String, Any?>> {
        val ctx = context ?: return emptyList()
        val contacts = mutableListOf<Map<String, Any?>>()
        
        val cursor = ctx.contentResolver.query(
            ContactsContract.Contacts.CONTENT_URI,
            null,
            ContactsContract.Contacts.DISPLAY_NAME + " LIKE ?",
            arrayOf("%$query%"),
            ContactsContract.Contacts.DISPLAY_NAME + " ASC"
        )
        
        cursor?.use {
            val idIndex = it.getColumnIndex(ContactsContract.Contacts._ID)
            val nameIndex = it.getColumnIndex(ContactsContract.Contacts.DISPLAY_NAME)
            val hasPhoneIndex = it.getColumnIndex(ContactsContract.Contacts.HAS_PHONE_NUMBER)
            
            while (it.moveToNext()) {
                val id = it.getString(idIndex)
                val name = it.getString(nameIndex)
                val hasPhone = it.getInt(hasPhoneIndex) > 0
                
                val phones = if (hasPhone) getPhoneNumbers(ctx, id) else emptyList()
                
                contacts.add(mapOf(
                    "id" to id,
                    "name" to name,
                    "phones" to phones
                ))
            }
        }
        
        return contacts
    }
    
    private fun getPhoneNumbers(ctx: Context, contactId: String): List<Map<String, String?>> {
        val phones = mutableListOf<Map<String, String?>>()
        
        val cursor = ctx.contentResolver.query(
            Phone.CONTENT_URI,
            null,
            Phone.CONTACT_ID + " = ?",
            arrayOf(contactId),
            null
        )
        
        cursor?.use {
            val numberIndex = it.getColumnIndex(Phone.NUMBER)
            val typeIndex = it.getColumnIndex(Phone.TYPE)
            val labelIndex = it.getColumnIndex(Phone.LABEL)
            
            while (it.moveToNext()) {
                val number = it.getString(numberIndex)
                val type = it.getInt(typeIndex)
                val label = it.getString(labelIndex)
                
                phones.add(mapOf(
                    "number" to number,
                    "type" to getPhoneTypeLabel(type),
                    "label" to label
                ))
            }
        }
        
        return phones
    }
    
    private fun getEmails(ctx: Context, contactId: String): List<Map<String, String?>> {
        val emails = mutableListOf<Map<String, String?>>()
        
        val cursor = ctx.contentResolver.query(
            Email.CONTENT_URI,
            null,
            Email.CONTACT_ID + " = ?",
            arrayOf(contactId),
            null
        )
        
        cursor?.use {
            val addressIndex = it.getColumnIndex(Email.ADDRESS)
            val typeIndex = it.getColumnIndex(Email.TYPE)
            
            while (it.moveToNext()) {
                val address = it.getString(addressIndex)
                val type = it.getInt(typeIndex)
                
                emails.add(mapOf(
                    "address" to address,
                    "type" to getEmailTypeLabel(type)
                ))
            }
        }
        
        return emails
    }
    
    private fun getAddresses(ctx: Context, contactId: String): List<Map<String, String?>> {
        val addresses = mutableListOf<Map<String, String?>>()
        
        val cursor = ctx.contentResolver.query(
            StructuredPostal.CONTENT_URI,
            null,
            StructuredPostal.CONTACT_ID + " = ?",
            arrayOf(contactId),
            null
        )
        
        cursor?.use {
            val formattedIndex = it.getColumnIndex(StructuredPostal.FORMATTED_ADDRESS)
            val typeIndex = it.getColumnIndex(StructuredPostal.TYPE)
            val streetIndex = it.getColumnIndex(StructuredPostal.STREET)
            val cityIndex = it.getColumnIndex(StructuredPostal.CITY)
            val countryIndex = it.getColumnIndex(StructuredPostal.COUNTRY)
            
            while (it.moveToNext()) {
                addresses.add(mapOf(
                    "formatted" to it.getString(formattedIndex),
                    "type" to getAddressTypeLabel(it.getInt(typeIndex)),
                    "street" to it.getString(streetIndex),
                    "city" to it.getString(cityIndex),
                    "country" to it.getString(countryIndex)
                ))
            }
        }
        
        return addresses
    }
    
    private fun getOrganizations(ctx: Context, contactId: String): List<Map<String, String?>> {
        val orgs = mutableListOf<Map<String, String?>>()
        
        val cursor = ctx.contentResolver.query(
            ContactsContract.Data.CONTENT_URI,
            null,
            ContactsContract.Data.CONTACT_ID + " = ? AND " +
                    ContactsContract.Data.MIMETYPE + " = ?",
            arrayOf(contactId, Organization.CONTENT_ITEM_TYPE),
            null
        )
        
        cursor?.use {
            val companyIndex = it.getColumnIndex(Organization.COMPANY)
            val titleIndex = it.getColumnIndex(Organization.TITLE)
            
            while (it.moveToNext()) {
                orgs.add(mapOf(
                    "company" to it.getString(companyIndex),
                    "title" to it.getString(titleIndex)
                ))
            }
        }
        
        return orgs
    }
    
    private fun getWebsites(ctx: Context, contactId: String): List<String> {
        val websites = mutableListOf<String>()
        
        val cursor = ctx.contentResolver.query(
            ContactsContract.Data.CONTENT_URI,
            null,
            ContactsContract.Data.CONTACT_ID + " = ? AND " +
                    ContactsContract.Data.MIMETYPE + " = ?",
            arrayOf(contactId, Website.CONTENT_ITEM_TYPE),
            null
        )
        
        cursor?.use {
            val urlIndex = it.getColumnIndex(Website.URL)
            
            while (it.moveToNext()) {
                it.getString(urlIndex)?.let { url -> websites.add(url) }
            }
        }
        
        return websites
    }
    
    private fun getNotes(ctx: Context, contactId: String): String? {
        val cursor = ctx.contentResolver.query(
            ContactsContract.Data.CONTENT_URI,
            null,
            ContactsContract.Data.CONTACT_ID + " = ? AND " +
                    ContactsContract.Data.MIMETYPE + " = ?",
            arrayOf(contactId, Note.CONTENT_ITEM_TYPE),
            null
        )
        
        cursor?.use {
            val noteIndex = it.getColumnIndex(Note.NOTE)
            if (it.moveToFirst()) {
                return it.getString(noteIndex)
            }
        }
        
        return null
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Add Contact
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun addContact(
        name: String,
        phone: String? = null,
        email: String? = null,
        company: String? = null
    ): Boolean {
        val ctx = context ?: return false
        
        val ops = ArrayList<ContentProviderOperation>()
        
        // إنشاء جهة اتصال جديدة
        ops.add(ContentProviderOperation.newInsert(ContactsContract.RawContacts.CONTENT_URI)
            .withValue(ContactsContract.RawContacts.ACCOUNT_TYPE, null)
            .withValue(ContactsContract.RawContacts.ACCOUNT_NAME, null)
            .build())
        
        // إضافة الاسم
        ops.add(ContentProviderOperation.newInsert(ContactsContract.Data.CONTENT_URI)
            .withValueBackReference(ContactsContract.Data.RAW_CONTACT_ID, 0)
            .withValue(ContactsContract.Data.MIMETYPE, StructuredName.CONTENT_ITEM_TYPE)
            .withValue(StructuredName.DISPLAY_NAME, name)
            .build())
        
        // إضافة رقم الهاتف
        phone?.let {
            ops.add(ContentProviderOperation.newInsert(ContactsContract.Data.CONTENT_URI)
                .withValueBackReference(ContactsContract.Data.RAW_CONTACT_ID, 0)
                .withValue(ContactsContract.Data.MIMETYPE, Phone.CONTENT_ITEM_TYPE)
                .withValue(Phone.NUMBER, it)
                .withValue(Phone.TYPE, Phone.TYPE_MOBILE)
                .build())
        }
        
        // إضافة البريد الإلكتروني
        email?.let {
            ops.add(ContentProviderOperation.newInsert(ContactsContract.Data.CONTENT_URI)
                .withValueBackReference(ContactsContract.Data.RAW_CONTACT_ID, 0)
                .withValue(ContactsContract.Data.MIMETYPE, Email.CONTENT_ITEM_TYPE)
                .withValue(Email.ADDRESS, it)
                .withValue(Email.TYPE, Email.TYPE_WORK)
                .build())
        }
        
        // إضافة الشركة
        company?.let {
            ops.add(ContentProviderOperation.newInsert(ContactsContract.Data.CONTENT_URI)
                .withValueBackReference(ContactsContract.Data.RAW_CONTACT_ID, 0)
                .withValue(ContactsContract.Data.MIMETYPE, Organization.CONTENT_ITEM_TYPE)
                .withValue(Organization.COMPANY, it)
                .build())
        }
        
        return try {
            ctx.contentResolver.applyBatch(ContactsContract.AUTHORITY, ops)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Delete Contact
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun deleteContact(contactId: String): Boolean {
        val ctx = context ?: return false
        
        val uri = Uri.withAppendedPath(ContactsContract.Contacts.CONTENT_URI, contactId)
        val rows = ctx.contentResolver.delete(uri, null, null)
        return rows > 0
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Statistics
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getContactCount(): Int {
        val ctx = context ?: return 0
        
        val cursor = ctx.contentResolver.query(
            ContactsContract.Contacts.CONTENT_URI,
            arrayOf(ContactsContract.Contacts._ID),
            null,
            null,
            null
        )
        
        val count = cursor?.count ?: 0
        cursor?.close()
        return count
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Helper Functions
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private fun getPhoneTypeLabel(type: Int): String {
        return when (type) {
            Phone.TYPE_HOME -> "المنزل"
            Phone.TYPE_MOBILE -> "الجوال"
            Phone.TYPE_WORK -> "العمل"
            Phone.TYPE_FAX_HOME -> "فاكس المنزل"
            Phone.TYPE_FAX_WORK -> "فاكس العمل"
            Phone.TYPE_MAIN -> "الرئيسي"
            Phone.TYPE_OTHER -> "آخر"
            else -> "آخر"
        }
    }
    
    private fun getEmailTypeLabel(type: Int): String {
        return when (type) {
            Email.TYPE_HOME -> "المنزل"
            Email.TYPE_WORK -> "العمل"
            Email.TYPE_OTHER -> "آخر"
            else -> "آخر"
        }
    }
    
    private fun getAddressTypeLabel(type: Int): String {
        return when (type) {
            StructuredPostal.TYPE_HOME -> "المنزل"
            StructuredPostal.TYPE_WORK -> "العمل"
            StructuredPostal.TYPE_OTHER -> "آخر"
            else -> "آخر"
        }
    }
}
