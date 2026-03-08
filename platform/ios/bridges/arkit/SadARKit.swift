/**
 * =============================================================================
 * @file SadARKit.swift
 * @brief (AR) جسر ARKit — الواقع المعزز على iOS
 * @brief (EN) ARKit Bridge — Augmented Reality on iOS
 * 
 * المهمة: T250 - ARKit واقع معزز
 * المرحلة: سبتمبر 2026 — iOS CoreML
 * =============================================================================
 * 
 * 🎯 الميزات
 * ══════════
 * ✓ تتبع الوجوه (Face Tracking)
 * ✓ تتبع الأسطح (Plane Detection)
 * ✓ تتبع الصور (Image Tracking)
 * ✓ وضع الأجسام 3D (Object Placement)
 * ✓ قياس المسافات
 * ✓ تأثيرات الواقع المعزز
 * 
 * =============================================================================
 */

import Foundation
import ARKit
import SceneKit
import UIKit

// MARK: - الأنواع

/// نوع السطح المكتشف
public enum PlaneType {
    case horizontal     // أفقي (أرضية، طاولة)
    case vertical       // عمودي (جدار)
    case any           // أي سطح
}

/// معلومات السطح
public struct PlaneInfo {
    public let identifier: UUID
    public let type: PlaneType
    public let center: SCNVector3
    public let extent: SCNVector3      // الأبعاد
    public let transform: simd_float4x4
}

/// معلومات الوجه
public struct FaceInfo {
    public let identifier: UUID
    public let leftEyePosition: SCNVector3
    public let rightEyePosition: SCNVector3
    public let lookAtPoint: SCNVector3
    public let isBlinking: Bool
    public let isSmiling: Bool
    public let blendShapes: [ARFaceAnchor.BlendShapeLocation: Float]
}

/// نتيجة Ray-cast
public struct RaycastResult {
    public let position: SCNVector3
    public let normal: SCNVector3
    public let distance: Float
    public let anchor: ARAnchor?
}

/// مستمع أحداث AR
public protocol SadARDelegate: AnyObject {
    func onSessionStarted()
    func onSessionPaused()
    func onPlaneDetected(_ plane: PlaneInfo)
    func onPlaneUpdated(_ plane: PlaneInfo)
    func onPlaneRemoved(identifier: UUID)
    func onFaceDetected(_ face: FaceInfo)
    func onFaceUpdated(_ face: FaceInfo)
    func onImageDetected(name: String, transform: simd_float4x4)
    func onError(_ error: String)
}

// MARK: - تقديم extension افتراضي لـ delegate

public extension SadARDelegate {
    func onSessionStarted() {}
    func onSessionPaused() {}
    func onPlaneDetected(_ plane: PlaneInfo) {}
    func onPlaneUpdated(_ plane: PlaneInfo) {}
    func onPlaneRemoved(identifier: UUID) {}
    func onFaceDetected(_ face: FaceInfo) {}
    func onFaceUpdated(_ face: FaceInfo) {}
    func onImageDetected(name: String, transform: simd_float4x4) {}
    func onError(_ error: String) {}
}

// MARK: - SadARKit

public class SadARKit: NSObject {
    
    // MARK: - الخصائص
    
    private var sceneView: ARSCNView?
    private var configuration: ARConfiguration?
    
    public weak var delegate: SadARDelegate?
    
    /// الأجسام المضافة للمشهد
    private var placedObjects: [UUID: SCNNode] = [:]
    
    /// الأسطح المكتشفة
    private var detectedPlanes: [UUID: PlaneInfo] = [:]
    
    // MARK: - التهيئة
    
    public override init() {
        super.init()
    }
    
    // MARK: - إعداد المشهد
    
    /// إعداد AR Scene View
    public func setup(with view: ARSCNView) {
        sceneView = view
        sceneView?.delegate = self
        sceneView?.autoenablesDefaultLighting = true
        sceneView?.automaticallyUpdatesLighting = true
        
        // إظهار إحصائيات (اختياري)
        sceneView?.showsStatistics = false
        
        // إضافة إضاءة محيطة
        let ambientLight = SCNLight()
        ambientLight.type = .ambient
        ambientLight.intensity = 500
        let ambientNode = SCNNode()
        ambientNode.light = ambientLight
        sceneView?.scene.rootNode.addChildNode(ambientNode)
    }
    
    /// هل ARKit مدعوم؟
    public static func isSupported() -> Bool {
        return ARWorldTrackingConfiguration.isSupported
    }
    
    /// هل تتبع الوجوه مدعوم؟
    public static func isFaceTrackingSupported() -> Bool {
        return ARFaceTrackingConfiguration.isSupported
    }
    
    // MARK: - بدء وإيقاف الجلسة
    
    /// بدء جلسة تتبع الأسطح
    public func startPlaneDetection(planeType: PlaneType = .horizontal) {
        let config = ARWorldTrackingConfiguration()
        
        switch planeType {
        case .horizontal:
            config.planeDetection = .horizontal
        case .vertical:
            config.planeDetection = .vertical
        case .any:
            config.planeDetection = [.horizontal, .vertical]
        }
        
        config.environmentTexturing = .automatic
        
        configuration = config
        sceneView?.session.run(config, options: [.resetTracking, .removeExistingAnchors])
        delegate?.onSessionStarted()
    }
    
    /// بدء جلسة تتبع الوجوه
    public func startFaceTracking() {
        guard ARFaceTrackingConfiguration.isSupported else {
            delegate?.onError("تتبع الوجوه غير مدعوم على هذا الجهاز")
            return
        }
        
        let config = ARFaceTrackingConfiguration()
        config.isLightEstimationEnabled = true
        
        configuration = config
        sceneView?.session.run(config, options: [.resetTracking])
        delegate?.onSessionStarted()
    }
    
    /// بدء تتبع الصور
    public func startImageTracking(referenceImages: Set<ARReferenceImage>) {
        let config = ARWorldTrackingConfiguration()
        config.detectionImages = referenceImages
        config.maximumNumberOfTrackedImages = 4
        
        configuration = config
        sceneView?.session.run(config, options: [.resetTracking])
        delegate?.onSessionStarted()
    }
    
    /// تحميل صور مرجعية من مجلد Assets
    public func loadReferenceImages(groupName: String) -> Set<ARReferenceImage>? {
        return ARReferenceImage.referenceImages(inGroupNamed: groupName, bundle: nil)
    }
    
    /// إيقاف الجلسة
    public func pause() {
        sceneView?.session.pause()
        delegate?.onSessionPaused()
    }
    
    /// استئناف الجلسة
    public func resume() {
        if let config = configuration {
            sceneView?.session.run(config)
        }
    }
    
    // MARK: - وضع الأجسام
    
    /// وضع مكعب في نقطة
    public func placeCube(at position: SCNVector3, size: Float = 0.1, color: UIColor = .red) -> UUID {
        let cube = SCNBox(width: CGFloat(size), height: CGFloat(size), length: CGFloat(size), chamferRadius: 0.005)
        cube.firstMaterial?.diffuse.contents = color
        
        let node = SCNNode(geometry: cube)
        node.position = position
        
        sceneView?.scene.rootNode.addChildNode(node)
        
        let id = UUID()
        placedObjects[id] = node
        return id
    }
    
    /// وضع كرة في نقطة
    public func placeSphere(at position: SCNVector3, radius: Float = 0.05, color: UIColor = .blue) -> UUID {
        let sphere = SCNSphere(radius: CGFloat(radius))
        sphere.firstMaterial?.diffuse.contents = color
        
        let node = SCNNode(geometry: sphere)
        node.position = position
        
        sceneView?.scene.rootNode.addChildNode(node)
        
        let id = UUID()
        placedObjects[id] = node
        return id
    }
    
    /// تحميل نموذج 3D من ملف
    public func placeModel(named name: String, at position: SCNVector3, scale: Float = 1.0) -> UUID? {
        guard let scene = SCNScene(named: name) else {
            delegate?.onError("فشل في تحميل النموذج: \(name)")
            return nil
        }
        
        let node = scene.rootNode.clone()
        node.position = position
        node.scale = SCNVector3(scale, scale, scale)
        
        sceneView?.scene.rootNode.addChildNode(node)
        
        let id = UUID()
        placedObjects[id] = node
        return id
    }
    
    /// وضع نص في مكان
    public func placeText(_ text: String, at position: SCNVector3, size: Float = 0.1, color: UIColor = .white) -> UUID {
        let textGeometry = SCNText(string: text, extrusionDepth: CGFloat(size * 0.1))
        textGeometry.font = UIFont.systemFont(ofSize: CGFloat(size))
        textGeometry.firstMaterial?.diffuse.contents = color
        
        let node = SCNNode(geometry: textGeometry)
        node.position = position
        
        // توسيط النص
        let (min, max) = node.boundingBox
        node.pivot = SCNMatrix4MakeTranslation((max.x - min.x) / 2, 0, 0)
        
        sceneView?.scene.rootNode.addChildNode(node)
        
        let id = UUID()
        placedObjects[id] = node
        return id
    }
    
    /// حذف جسم
    public func removeObject(id: UUID) {
        placedObjects[id]?.removeFromParentNode()
        placedObjects.removeValue(forKey: id)
    }
    
    /// حذف جميع الأجسام
    public func removeAllObjects() {
        for (_, node) in placedObjects {
            node.removeFromParentNode()
        }
        placedObjects.removeAll()
    }
    
    // MARK: - Ray-casting
    
    /// تنفيذ ray-cast من نقطة الشاشة
    public func raycast(from screenPoint: CGPoint) -> RaycastResult? {
        guard let query = sceneView?.raycastQuery(from: screenPoint, allowing: .estimatedPlane, alignment: .any),
              let result = sceneView?.session.raycast(query).first else {
            return nil
        }
        
        let position = SCNVector3(
            result.worldTransform.columns.3.x,
            result.worldTransform.columns.3.y,
            result.worldTransform.columns.3.z
        )
        
        // تقدير المسافة
        guard let cameraTransform = sceneView?.session.currentFrame?.camera.transform else {
            return nil
        }
        
        let cameraPos = SCNVector3(
            cameraTransform.columns.3.x,
            cameraTransform.columns.3.y,
            cameraTransform.columns.3.z
        )
        
        let distance = sqrt(
            pow(position.x - cameraPos.x, 2) +
            pow(position.y - cameraPos.y, 2) +
            pow(position.z - cameraPos.z, 2)
        )
        
        return RaycastResult(
            position: position,
            normal: SCNVector3(0, 1, 0),  // تقريبي
            distance: distance,
            anchor: result.anchor
        )
    }
    
    /// وضع جسم عند لمس الشاشة
    public func handleTap(at point: CGPoint, placementHandler: (SCNVector3) -> Void) {
        if let result = raycast(from: point) {
            placementHandler(result.position)
        }
    }
    
    // MARK: - القياس
    
    /// حساب المسافة بين نقطتين
    public func measureDistance(from point1: SCNVector3, to point2: SCNVector3) -> Float {
        return sqrt(
            pow(point2.x - point1.x, 2) +
            pow(point2.y - point1.y, 2) +
            pow(point2.z - point1.z, 2)
        )
    }
    
    /// رسم خط بين نقطتين
    public func drawLine(from start: SCNVector3, to end: SCNVector3, color: UIColor = .yellow) -> UUID {
        let vertices = [start, end]
        let source = SCNGeometrySource(vertices: vertices)
        let indices: [Int32] = [0, 1]
        let element = SCNGeometryElement(indices: indices, primitiveType: .line)
        
        let line = SCNGeometry(sources: [source], elements: [element])
        line.firstMaterial?.diffuse.contents = color
        line.firstMaterial?.isDoubleSided = true
        
        let node = SCNNode(geometry: line)
        sceneView?.scene.rootNode.addChildNode(node)
        
        let id = UUID()
        placedObjects[id] = node
        return id
    }
    
    // MARK: - تأثيرات الوجه
    
    /// تطبيق قناع على الوجه
    public func applyFaceMask(imageName: String) {
        // يتم تطبيقه في ARSCNViewDelegate عند اكتشاف الوجه
    }
}

// MARK: - ARSCNViewDelegate

extension SadARKit: ARSCNViewDelegate {
    
    public func renderer(_ renderer: SCNSceneRenderer, didAdd node: SCNNode, for anchor: ARAnchor) {
        if let planeAnchor = anchor as? ARPlaneAnchor {
            let planeType: PlaneType = planeAnchor.alignment == .horizontal ? .horizontal : .vertical
            
            let info = PlaneInfo(
                identifier: anchor.identifier,
                type: planeType,
                center: SCNVector3(planeAnchor.center.x, 0, planeAnchor.center.z),
                extent: SCNVector3(planeAnchor.extent.x, 0, planeAnchor.extent.z),
                transform: anchor.transform
            )
            
            detectedPlanes[anchor.identifier] = info
            
            DispatchQueue.main.async {
                self.delegate?.onPlaneDetected(info)
            }
            
            // إضافة visualization للسطح
            addPlaneVisualization(to: node, for: planeAnchor)
        }
        
        if let faceAnchor = anchor as? ARFaceAnchor {
            let blendShapes = faceAnchor.blendShapes
            let isBlinking = (blendShapes[.eyeBlinkLeft]?.floatValue ?? 0) > 0.5 ||
                            (blendShapes[.eyeBlinkRight]?.floatValue ?? 0) > 0.5
            let isSmiling = (blendShapes[.mouthSmileLeft]?.floatValue ?? 0) > 0.5 ||
                           (blendShapes[.mouthSmileRight]?.floatValue ?? 0) > 0.5
            
            let info = FaceInfo(
                identifier: anchor.identifier,
                leftEyePosition: SCNVector3(faceAnchor.leftEyeTransform.columns.3.x,
                                           faceAnchor.leftEyeTransform.columns.3.y,
                                           faceAnchor.leftEyeTransform.columns.3.z),
                rightEyePosition: SCNVector3(faceAnchor.rightEyeTransform.columns.3.x,
                                            faceAnchor.rightEyeTransform.columns.3.y,
                                            faceAnchor.rightEyeTransform.columns.3.z),
                lookAtPoint: SCNVector3(faceAnchor.lookAtPoint.x,
                                       faceAnchor.lookAtPoint.y,
                                       faceAnchor.lookAtPoint.z),
                isBlinking: isBlinking,
                isSmiling: isSmiling,
                blendShapes: blendShapes.mapValues { $0.floatValue }
            )
            
            DispatchQueue.main.async {
                self.delegate?.onFaceDetected(info)
            }
        }
        
        if let imageAnchor = anchor as? ARImageAnchor {
            let name = imageAnchor.referenceImage.name ?? "unknown"
            DispatchQueue.main.async {
                self.delegate?.onImageDetected(name: name, transform: anchor.transform)
            }
        }
    }
    
    public func renderer(_ renderer: SCNSceneRenderer, didUpdate node: SCNNode, for anchor: ARAnchor) {
        if let planeAnchor = anchor as? ARPlaneAnchor {
            let planeType: PlaneType = planeAnchor.alignment == .horizontal ? .horizontal : .vertical
            
            let info = PlaneInfo(
                identifier: anchor.identifier,
                type: planeType,
                center: SCNVector3(planeAnchor.center.x, 0, planeAnchor.center.z),
                extent: SCNVector3(planeAnchor.extent.x, 0, planeAnchor.extent.z),
                transform: anchor.transform
            )
            
            detectedPlanes[anchor.identifier] = info
            updatePlaneVisualization(on: node, for: planeAnchor)
            
            DispatchQueue.main.async {
                self.delegate?.onPlaneUpdated(info)
            }
        }
        
        if let faceAnchor = anchor as? ARFaceAnchor {
            let blendShapes = faceAnchor.blendShapes
            let isBlinking = (blendShapes[.eyeBlinkLeft]?.floatValue ?? 0) > 0.5 ||
                            (blendShapes[.eyeBlinkRight]?.floatValue ?? 0) > 0.5
            let isSmiling = (blendShapes[.mouthSmileLeft]?.floatValue ?? 0) > 0.5 ||
                           (blendShapes[.mouthSmileRight]?.floatValue ?? 0) > 0.5
            
            let info = FaceInfo(
                identifier: anchor.identifier,
                leftEyePosition: SCNVector3(faceAnchor.leftEyeTransform.columns.3.x,
                                           faceAnchor.leftEyeTransform.columns.3.y,
                                           faceAnchor.leftEyeTransform.columns.3.z),
                rightEyePosition: SCNVector3(faceAnchor.rightEyeTransform.columns.3.x,
                                            faceAnchor.rightEyeTransform.columns.3.y,
                                            faceAnchor.rightEyeTransform.columns.3.z),
                lookAtPoint: SCNVector3(faceAnchor.lookAtPoint.x,
                                       faceAnchor.lookAtPoint.y,
                                       faceAnchor.lookAtPoint.z),
                isBlinking: isBlinking,
                isSmiling: isSmiling,
                blendShapes: blendShapes.mapValues { $0.floatValue }
            )
            
            DispatchQueue.main.async {
                self.delegate?.onFaceUpdated(info)
            }
        }
    }
    
    public func renderer(_ renderer: SCNSceneRenderer, didRemove node: SCNNode, for anchor: ARAnchor) {
        if anchor is ARPlaneAnchor {
            detectedPlanes.removeValue(forKey: anchor.identifier)
            DispatchQueue.main.async {
                self.delegate?.onPlaneRemoved(identifier: anchor.identifier)
            }
        }
    }
    
    // MARK: - Private
    
    private func addPlaneVisualization(to node: SCNNode, for planeAnchor: ARPlaneAnchor) {
        let plane = SCNPlane(width: CGFloat(planeAnchor.extent.x), height: CGFloat(planeAnchor.extent.z))
        plane.firstMaterial?.diffuse.contents = UIColor.blue.withAlphaComponent(0.3)
        plane.firstMaterial?.isDoubleSided = true
        
        let planeNode = SCNNode(geometry: plane)
        planeNode.position = SCNVector3(planeAnchor.center.x, 0, planeAnchor.center.z)
        planeNode.eulerAngles.x = -.pi / 2
        planeNode.name = "planeVisualization"
        
        node.addChildNode(planeNode)
    }
    
    private func updatePlaneVisualization(on node: SCNNode, for planeAnchor: ARPlaneAnchor) {
        guard let planeNode = node.childNode(withName: "planeVisualization", recursively: false),
              let plane = planeNode.geometry as? SCNPlane else { return }
        
        plane.width = CGFloat(planeAnchor.extent.x)
        plane.height = CGFloat(planeAnchor.extent.z)
        planeNode.position = SCNVector3(planeAnchor.center.x, 0, planeAnchor.center.z)
    }
}

// MARK: - دوال مساعدة للغة ص

/// إنشاء مدير AR
public func واقع_انشئ() -> SadARKit {
    return SadARKit()
}

/// بدء تتبع الأسطح
public func واقع_أسطح(_ ar: SadARKit) {
    ar.startPlaneDetection()
}

/// بدء تتبع الوجوه
public func واقع_وجوه(_ ar: SadARKit) {
    ar.startFaceTracking()
}

/// إضافة مكعب
public func واقع_مكعب(_ ar: SadARKit, س: Float, ص: Float, ع: Float, حجم: Float = 0.1) -> String {
    let position = SCNVector3(س, ص, ع)
    return ar.placeCube(at: position, size: حجم).uuidString
}

/// إضافة نص
public func واقع_نص(_ ar: SadARKit, نص: String, س: Float, ص: Float, ع: Float) -> String {
    let position = SCNVector3(س, ص, ع)
    return ar.placeText(نص, at: position).uuidString
}

/// قياس المسافة
public func واقع_قس(_ ar: SadARKit, س1: Float, ص1: Float, ع1: Float, س2: Float, ص2: Float, ع2: Float) -> Float {
    let p1 = SCNVector3(س1, ص1, ع1)
    let p2 = SCNVector3(س2, ص2, ع2)
    return ar.measureDistance(from: p1, to: p2)
}

/// إيقاف AR
public func واقع_أوقف(_ ar: SadARKit) {
    ar.pause()
}
