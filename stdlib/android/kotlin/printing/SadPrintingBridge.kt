// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadPrintingBridge.kt
// الوصف: جسر Kotlin للطباعة
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.pdf.PdfDocument
import android.os.Build
import android.os.Bundle
import android.os.CancellationSignal
import android.os.ParcelFileDescriptor
import android.print.*
import android.print.pdf.PrintedPdfDocument
import android.webkit.WebView
import android.webkit.WebViewClient
import java.io.File
import java.io.FileInputStream
import java.io.FileOutputStream

/**
 * جسر الطباعة — SadPrintingBridge
 */
object SadPrintingBridge {
    
    private var context: Context? = null
    private var printManager: PrintManager? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        printManager = ctx.getSystemService(Context.PRINT_SERVICE) as? PrintManager
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Print HTML
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun printHtml(
        html: String,
        jobName: String,
        baseUrl: String? = null
    ): Boolean {
        val ctx = context ?: return false
        
        return try {
            val webView = WebView(ctx)
            webView.webViewClient = object : WebViewClient() {
                override fun onPageFinished(view: WebView?, url: String?) {
                    createWebPrintJob(view ?: return, jobName)
                }
            }
            
            if (baseUrl != null) {
                webView.loadDataWithBaseURL(baseUrl, html, "text/html", "UTF-8", null)
            } else {
                webView.loadData(html, "text/html", "UTF-8")
            }
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun printArabicHtml(
        html: String,
        jobName: String
    ): Boolean {
        val rtlHtml = """
            <!DOCTYPE html>
            <html dir="rtl" lang="ar">
            <head>
                <meta charset="UTF-8">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <style>
                    body {
                        font-family: 'Amiri', 'Traditional Arabic', 'Noto Naskh Arabic', serif;
                        direction: rtl;
                        text-align: right;
                        line-height: 1.8;
                    }
                </style>
            </head>
            <body>
                $html
            </body>
            </html>
        """.trimIndent()
        
        return printHtml(rtlHtml, jobName)
    }
    
    private fun createWebPrintJob(webView: WebView, jobName: String) {
        val printAdapter = webView.createPrintDocumentAdapter(jobName)
        printManager?.print(jobName, printAdapter, null)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Print URL
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun printUrl(
        url: String,
        jobName: String
    ): Boolean {
        val ctx = context ?: return false
        
        return try {
            val webView = WebView(ctx)
            webView.webViewClient = object : WebViewClient() {
                override fun onPageFinished(view: WebView?, url: String?) {
                    createWebPrintJob(view ?: return, jobName)
                }
            }
            
            webView.loadUrl(url)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Print Bitmap
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun printBitmap(
        bitmap: Bitmap,
        jobName: String,
        scaleMode: Int = PrintHelper.SCALE_MODE_FIT
    ): Boolean {
        val ctx = context ?: return false
        
        return try {
            val printHelper = PrintHelper(ctx)
            printHelper.scaleMode = scaleMode
            printHelper.printBitmap(jobName, bitmap)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun printBitmapFill(bitmap: Bitmap, jobName: String): Boolean {
        return printBitmap(bitmap, jobName, PrintHelper.SCALE_MODE_FILL)
    }
    
    @JvmStatic
    fun printBitmapFit(bitmap: Bitmap, jobName: String): Boolean {
        return printBitmap(bitmap, jobName, PrintHelper.SCALE_MODE_FIT)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Print PDF
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun printPdf(
        pdfPath: String,
        jobName: String
    ): Boolean {
        val pm = printManager ?: return false
        
        return try {
            val file = File(pdfPath)
            if (!file.exists()) return false
            
            val printAdapter = PdfPrintAdapter(file)
            pm.print(jobName, printAdapter, null)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    private class PdfPrintAdapter(private val pdfFile: File) : PrintDocumentAdapter() {
        
        override fun onLayout(
            oldAttributes: PrintAttributes?,
            newAttributes: PrintAttributes?,
            cancellationSignal: CancellationSignal?,
            callback: LayoutResultCallback?,
            extras: Bundle?
        ) {
            if (cancellationSignal?.isCanceled == true) {
                callback?.onLayoutCancelled()
                return
            }
            
            val info = PrintDocumentInfo.Builder(pdfFile.name)
                .setContentType(PrintDocumentInfo.CONTENT_TYPE_DOCUMENT)
                .setPageCount(PrintDocumentInfo.PAGE_COUNT_UNKNOWN)
                .build()
            
            callback?.onLayoutFinished(info, true)
        }
        
        override fun onWrite(
            pages: Array<out PageRange>?,
            destination: ParcelFileDescriptor?,
            cancellationSignal: CancellationSignal?,
            callback: WriteResultCallback?
        ) {
            try {
                FileInputStream(pdfFile).use { input ->
                    FileOutputStream(destination?.fileDescriptor).use { output ->
                        input.copyTo(output)
                    }
                }
                
                callback?.onWriteFinished(arrayOf(PageRange.ALL_PAGES))
            } catch (e: Exception) {
                callback?.onWriteFailed(e.message)
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Custom Print
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun printCustomDocument(
        jobName: String,
        pageWidth: Int,
        pageHeight: Int,
        drawPage: (Canvas, Int, Int, Int) -> Int // canvas, pageNum, width, height -> totalPages
    ): Boolean {
        val pm = printManager ?: return false
        
        return try {
            val printAdapter = object : PrintDocumentAdapter() {
                private var totalPages = 0
                
                override fun onLayout(
                    oldAttributes: PrintAttributes?,
                    newAttributes: PrintAttributes?,
                    cancellationSignal: CancellationSignal?,
                    callback: LayoutResultCallback?,
                    extras: Bundle?
                ) {
                    if (cancellationSignal?.isCanceled == true) {
                        callback?.onLayoutCancelled()
                        return
                    }
                    
                    val info = PrintDocumentInfo.Builder(jobName)
                        .setContentType(PrintDocumentInfo.CONTENT_TYPE_DOCUMENT)
                        .setPageCount(PrintDocumentInfo.PAGE_COUNT_UNKNOWN)
                        .build()
                    
                    callback?.onLayoutFinished(info, true)
                }
                
                override fun onWrite(
                    pages: Array<out PageRange>?,
                    destination: ParcelFileDescriptor?,
                    cancellationSignal: CancellationSignal?,
                    callback: WriteResultCallback?
                ) {
                    try {
                        val pdfDocument = PdfDocument()
                        
                        var pageNum = 0
                        var hasMorePages = true
                        
                        while (hasMorePages) {
                            if (cancellationSignal?.isCanceled == true) {
                                callback?.onWriteCancelled()
                                pdfDocument.close()
                                return
                            }
                            
                            val pageInfo = PdfDocument.PageInfo.Builder(pageWidth, pageHeight, pageNum).create()
                            val page = pdfDocument.startPage(pageInfo)
                            
                            totalPages = drawPage(page.canvas, pageNum, pageWidth, pageHeight)
                            
                            pdfDocument.finishPage(page)
                            
                            pageNum++
                            hasMorePages = pageNum < totalPages
                        }
                        
                        FileOutputStream(destination?.fileDescriptor).use { output ->
                            pdfDocument.writeTo(output)
                        }
                        
                        pdfDocument.close()
                        callback?.onWriteFinished(arrayOf(PageRange.ALL_PAGES))
                    } catch (e: Exception) {
                        callback?.onWriteFailed(e.message)
                    }
                }
            }
            
            pm.print(jobName, printAdapter, null)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Print Jobs
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getPrintJobs(): List<Map<String, Any?>> {
        val pm = printManager ?: return emptyList()
        
        return pm.printJobs.map { job ->
            mapOf(
                "id" to job.id.toString(),
                "label" to job.info.label,
                "state" to getPrintJobStateLabel(job.info.state),
                "copies" to job.info.copies,
                "creationTime" to job.info.creationTime,
                "printerName" to job.info.printerId?.localId
            )
        }
    }
    
    @JvmStatic
    fun cancelPrintJob(jobId: String): Boolean {
        val pm = printManager ?: return false
        
        return try {
            val job = pm.printJobs.find { it.id.toString() == jobId }
            job?.cancel()
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun restartPrintJob(jobId: String): Boolean {
        val pm = printManager ?: return false
        
        return try {
            val job = pm.printJobs.find { it.id.toString() == jobId }
            job?.restart()
            true
        } catch (e: Exception) {
            false
        }
    }
    
    private fun getPrintJobStateLabel(state: Int): String {
        return when (state) {
            PrintJobInfo.STATE_CREATED -> "تم الإنشاء"
            PrintJobInfo.STATE_QUEUED -> "في قائمة الانتظار"
            PrintJobInfo.STATE_STARTED -> "بدأت"
            PrintJobInfo.STATE_BLOCKED -> "محجوبة"
            PrintJobInfo.STATE_COMPLETED -> "مكتملة"
            PrintJobInfo.STATE_FAILED -> "فشلت"
            PrintJobInfo.STATE_CANCELED -> "ملغية"
            else -> "غير معروفة"
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Utility
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isPrintingSupported(): Boolean {
        return printManager != null
    }
    
    @JvmStatic
    fun getDefaultPrintAttributes(): PrintAttributes {
        return PrintAttributes.Builder()
            .setMediaSize(PrintAttributes.MediaSize.ISO_A4)
            .setResolution(PrintAttributes.Resolution("default", "Default", 300, 300))
            .setMinMargins(PrintAttributes.Margins.NO_MARGINS)
            .build()
    }
    
    @JvmStatic
    fun getAvailableMediaSizes(): List<Map<String, Any>> {
        return listOf(
            mapOf("id" to "A4", "name" to "A4", "nameAr" to "أ4"),
            mapOf("id" to "A3", "name" to "A3", "nameAr" to "أ3"),
            mapOf("id" to "A5", "name" to "A5", "nameAr" to "أ5"),
            mapOf("id" to "LETTER", "name" to "Letter", "nameAr" to "رسالة"),
            mapOf("id" to "LEGAL", "name" to "Legal", "nameAr" to "قانوني"),
            mapOf("id" to "TABLOID", "name" to "Tabloid", "nameAr" to "تابلويد")
        )
    }
}
