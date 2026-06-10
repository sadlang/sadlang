"""
اختبار T1 — التحقق من تطابق ملفات YAML مع JSON Schemas المقابلة.

هذا الاختبار هو بوابة الجودة الأولى (T1 Schema Validation).
يُضاف كل نطاق جديد إلى قائمة YAML_SCHEMA_PAIRS عند اكتمال ستوري M1 المقابلة.

الإطار: pytest + jsonschema
التشغيل: pytest language-truth/tests/test_schema_validation.py -v
"""

import json
import pathlib
import pytest
import yaml

# ─── المسارات الأساسية ───
ROOT = pathlib.Path(__file__).parent.parent.parent  # جذر المشروع
TRUTH_DIR = ROOT / "language-truth"
SCHEMAS_DIR = TRUTH_DIR / "_schemas"


def load_yaml(path: pathlib.Path) -> object:
    """تحميل ملف YAML مع دعم UTF-8."""
    with open(path, encoding="utf-8") as f:
        return yaml.safe_load(f)


def load_json(path: pathlib.Path) -> dict:
    """تحميل ملف JSON مع دعم UTF-8."""
    with open(path, encoding="utf-8") as f:
        return json.load(f)


# ─── قائمة الأزواج (YAML، Schema) — تُوسَّع مع كل ستوري M1 ───
# الصيغة: (وصف_الاختبار, مسار_YAML_النسبي, مسار_Schema_النسبي, نوع_البيانات)
#   نوع البيانات: "list" إذا كان الملف يحوي قائمة entries، "single" لكائن واحد
YAML_SCHEMA_PAIRS: list[tuple[str, str, str, str]] = [
    # M0 — الكلمات المفتاحية
    # keywords.yaml بنية مختلفة (categories لا entries)، يُختبر بشكل خاص
    # سيُضاف المزيد هنا مع M1
]


class TestSchemaValidation:
    """اختبارات T1 — التحقق من صحة YAML ضد Schemas المقابلة."""

    def test_keywords_yaml_loads_cleanly(self):
        """keywords.yaml يُحمَّل بلا أخطاء ويحوي الهيكل المطلوب."""
        kw_path = TRUTH_DIR / "keywords.yaml"
        assert kw_path.exists(), f"keywords.yaml غير موجود: {kw_path}"
        data = load_yaml(kw_path)
        assert data is not None, "keywords.yaml فارغ"
        assert "version" in data, "keywords.yaml يفتقر لحقل version"
        assert "categories" in data, "keywords.yaml يفتقر لحقل categories"

    def test_keywords_yaml_version_is_v5(self):
        """keywords.yaml يحمل إصدار V5."""
        data = load_yaml(TRUTH_DIR / "keywords.yaml")
        assert data["version"].startswith("5."), \
            f"الإصدار المتوقع 5.x، الموجود: {data['version']}"

    def test_keywords_has_reserved_category(self):
        """keywords.yaml يحوي فئة reserved بـ 40 كلمة محجوزة."""
        data = load_yaml(TRUTH_DIR / "keywords.yaml")
        reserved = data["categories"].get("reserved", {})
        assert "keywords" in reserved, "فئة reserved لا تحوي قائمة keywords"
        count = len(reserved["keywords"])
        assert count >= 40, f"فئة reserved تحوي {count} كلمة — المتوقع ≥ 40"

    def test_keywords_has_operators_category(self):
        """keywords.yaml يحوي فئة operators بـ 3 عوامل منطقية."""
        data = load_yaml(TRUTH_DIR / "keywords.yaml")
        ops = data["categories"].get("operators", {})
        assert "keywords" in ops, "فئة operators لا تحوي قائمة keywords"
        count = len(ops["keywords"])
        assert count == 3, f"فئة operators تحوي {count} عوامل — المتوقع 3 (و، أو، ليس)"

    def test_keywords_has_builtin_types(self):
        """keywords.yaml يحوي فئة builtin_types بـ 9 أنواع مدمجة."""
        data = load_yaml(TRUTH_DIR / "keywords.yaml")
        types = data["categories"].get("builtin_types", {})
        assert "keywords" in types, "فئة builtin_types لا تحوي قائمة keywords"
        count = len(types["keywords"])
        assert count == 9, f"فئة builtin_types تحوي {count} — المتوقع 9"

    def test_keywords_total_count(self):
        """keywords.yaml يحوي ~74 كلمة إجمالاً (40 محجوزة + 25+ سياقية + 9 أنواع + 3 عوامل)."""
        data = load_yaml(TRUTH_DIR / "keywords.yaml")
        cats = data["categories"]
        total = sum(
            len(cat.get("keywords", []))
            for cat in cats.values()
            if isinstance(cat, dict)
        )
        assert total >= 74, f"إجمالي الكلمات {total} — المتوقع ≥ 74"

    def test_all_reserved_have_token_type(self):
        """كل كلمة محجوزة تحوي حقل tokenType."""
        data = load_yaml(TRUTH_DIR / "keywords.yaml")
        reserved = data["categories"]["reserved"]["keywords"]
        missing = [kw["word"] for kw in reserved if "tokenType" not in kw]
        assert not missing, f"كلمات محجوزة بدون tokenType: {missing}"

    def test_keywords_schema_is_valid_json(self):
        """keywords.schema.json ملف JSON صالح."""
        schema_path = SCHEMAS_DIR / "keywords.schema.json"
        assert schema_path.exists(), "keywords.schema.json غير موجود"
        schema = load_json(schema_path)
        assert "$schema" in schema or "$id" in schema, "Schema لا يحوي $schema أو $id"

    @pytest.mark.parametrize("schema_name", [
        "keywords.schema.json",
        "operator.schema.json",
        "directive.schema.json",
        "type.schema.json",
        "error.schema.json",
        "grammar_rule.schema.json",
        "stdlib_module.schema.json",
        "stdlib_function.schema.json",
        "builtin_function.schema.json",
    ])
    def test_schema_file_exists_and_valid_json(self, schema_name: str):
        """كل Schema موجود وهو JSON صالح."""
        path = SCHEMAS_DIR / schema_name
        assert path.exists(), f"Schema مفقود: {schema_name}"
        schema = load_json(path)
        assert isinstance(schema, dict), f"Schema ليس كائن JSON: {schema_name}"

    @pytest.mark.parametrize("schema_name", [
        "operator.schema.json",
        "directive.schema.json",
        "type.schema.json",
        "error.schema.json",
        "grammar_rule.schema.json",
        "stdlib_module.schema.json",
        "stdlib_function.schema.json",
    ])
    def test_v5_schema_has_required_fields(self, schema_name: str):
        """كل Schema V5 يحوي CommonFields المطلوبة."""
        schema = load_json(SCHEMAS_DIR / schema_name)
        required = schema.get("required", [])
        # CommonFields الإلزامية لكل Schema V5
        assert "id" in required, f"{schema_name}: id مفقود من required"
        assert "schema_version" in required, f"{schema_name}: schema_version مفقود"
        assert "since" in required, f"{schema_name}: since مفقود"
        assert "status" in required, f"{schema_name}: status مفقود"

    def test_meta_version_exists(self):
        """_meta/_version.yaml موجود ويحوي إصدار 5.0.0."""
        path = TRUTH_DIR / "_meta" / "_version.yaml"
        assert path.exists(), "_meta/_version.yaml غير موجود"
        data = load_yaml(path)
        assert data["version"] == "5.0.0", f"الإصدار المتوقع 5.0.0، الموجود: {data['version']}"

    def test_truth_dir_structure(self):
        """language-truth/ يحوي المجلدات الأساسية المطلوبة."""
        required_dirs = ["_schemas", "_meta", "builtins", "errors", "stdlib", "learning"]
        for d in required_dirs:
            path = TRUTH_DIR / d
            assert path.is_dir(), f"المجلد المطلوب غير موجود: language-truth/{d}/"

    @pytest.mark.parametrize("yaml_path,schema_name", YAML_SCHEMA_PAIRS)
    def test_yaml_validates_against_schema(self, yaml_path: str, schema_name: str):
        """كل ملف YAML يتطابق مع Schema المقابل (يُوسَّع مع M1)."""
        import jsonschema
        data = load_yaml(TRUTH_DIR / yaml_path)
        schema = load_json(SCHEMAS_DIR / schema_name)
        # التحقق من كل عنصر في القائمة
        if isinstance(data, list):
            for entry in data:
                jsonschema.validate(entry, schema)
        elif isinstance(data, dict) and "entries" in data:
            for entry in data["entries"]:
                jsonschema.validate(entry, schema)
        else:
            jsonschema.validate(data, schema)
