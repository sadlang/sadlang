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
# (AR) 🔑 كانت هذه القائمةُ **فارغةً منذ M0**، ورتبةُ صفِّها لا توافقُ
#      مُستهلِكَها: الإعلانُ رباعيٌّ و`parametrize` أدناه كان يطلبُ اسمَين. فاختبارُ
#      «كلُّ YAML يوافقُ مخطَّطَه» لم يجرِ قطُّ ولا على ملفٍّ واحد — وهو أخضرُ لأنّ
#      شرطَه لا يمكنُ أن يكونَ كاذبًا. ونتيجتُه المقيسةُ (٢٠٢٦-٠٩-٠٣): `operators.yaml`
#      انجرفَ عن مخطَّطِه حتّى صارَ **٤٣ مدخلًا من ٤٣ مخالِفًا**.
# (EN) This list sat EMPTY since M0 and its arity disagreed with its consumer, so
#      "every YAML matches its schema" never ran for any file — green because the
#      condition could not be false. Measured result: operators.yaml had drifted to
#      43/43 schema violations.
#
# (AR) الصيغة: (وصف، مسار YAML، اسم المخطَّط، مفتاح قائمة المدخلات أو None لكائنٍ واحد)
YAML_SCHEMA_PAIRS: list[tuple[str, str, str, str | None]] = [
    # M0 — الكلمات المفتاحية
    # keywords.yaml بنية مختلفة (categories لا entries)، يُختبر بشكل خاص
    ("العوامل", "operators.yaml", "operator.schema.json", "operators"),
    ("الأنواع", "types.yaml", "type.schema.json", "types"),
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
        """كل Schema V5 يحوي CommonFields المطلوبة على مستوى المدخلة.

        (AR) 🔑 كان هنا شرطٌ خامسٌ: «schema_version في required». وهو
             يطلبُ عُرفًا **لا تتّبعُه بياناتُ المشروع**، والدليلُ في الشجرةِ نفسِها:
             `type.schema.json` — وهو المخطَّطُ الوحيدُ المُصادَقُ فعلًا في بوّابةٍ
             حيّة — لا يعرفُ `schema_version` في خصائصِه أصلًا، و`types.yaml`
             يحملُه في **رأسِ الملفّ** لا في كلِّ مدخلة. فالإصدارُ صفةُ الملفِّ
             لا صفةُ المدخلة.

             وكان الشرطُ أحمرَ على `type.schema.json` و`error.schema.json` قبلَ
             هذا التعديلِ ولم يُلحَظْ، لأنّ CI لا يُشغّلُ `language-truth/tests/`
             إطلاقًا. فالتصحيحُ يُخضِرُ حمرتَين قائمتَين ولا يُخفيهما.
        (EN) A fifth clause demanded `schema_version` inside entry-level `required`
             — a convention the project's own data does not follow: type.schema.json,
             the only schema validated by a live gate, has no such property at all
             and types.yaml carries it at FILE level. The clause was already red for
             type/error and unnoticed because CI never runs this directory.
        """
        schema = load_json(SCHEMAS_DIR / schema_name)
        required = schema.get("required", [])
        # CommonFields الإلزامية لكل Schema V5 (على مستوى المدخلة)
        assert "id" in required, f"{schema_name}: id مفقود من required"
        assert "since" in required, f"{schema_name}: since مفقود"
        assert "status" in required, f"{schema_name}: status مفقود"

    @pytest.mark.parametrize("desc,yaml_path,schema_name,entries_key", YAML_SCHEMA_PAIRS)
    def test_sot_file_declares_schema_version(self, desc: str, yaml_path: str,
                                              schema_name: str, entries_key: "str | None"):
        """(AR) الإصدارُ صفةُ الملفِّ: كلُّ ملفٍّ مُصادَقٍ يُعلِنُ `schema_version` في رأسِه."""
        data = load_yaml(TRUTH_DIR / yaml_path)
        assert isinstance(data, dict), f"{desc}: {yaml_path} ليس خريطةً عليا"
        assert "schema_version" in data, \
            f"{desc}: {yaml_path} يفتقر schema_version في رأس الملفّ"
        assert str(data["schema_version"]).startswith("5."), \
            f"{desc}: schema_version المتوقع 5.x، الموجود: {data['schema_version']}"

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

    @pytest.mark.parametrize("desc,yaml_path,schema_name,entries_key", YAML_SCHEMA_PAIRS)
    def test_yaml_validates_against_schema(self, desc: str, yaml_path: str,
                                           schema_name: str, entries_key: "str | None"):
        """كل ملف YAML يتطابق مع Schema المقابل.

        (AR) `entries_key` يسمّي المفتاحَ الحاملَ للمدخلات صراحةً. وكان الاستدلالُ
             عليه ضمنيًّا («entries» أو قائمةٌ عليا)، وهو ما يجعلُ ملفًّا كـ
             `operators.yaml` — مدخلاتُه تحتَ `operators` — يُصادَقُ **بكامله**
             ضدّ مخطَّطِ المدخلةِ الواحدة، فيفشلُ لسببٍ لا علاقةَ له بالبيانات.
        """
        import jsonschema
        data = load_yaml(TRUTH_DIR / yaml_path)
        schema = load_json(SCHEMAS_DIR / schema_name)
        if entries_key is None:
            jsonschema.validate(data, schema)
            return
        entries = data[entries_key] if isinstance(data, dict) else data
        assert entries, f"{desc}: لا مدخلاتِ تحت «{entries_key}» في {yaml_path}"
        for entry in entries:
            jsonschema.validate(entry, schema)


# ─── طبقة الخلفيّة السياديّة الجدوليّة (م٠ من RFC الخلفيّة متعدّدة المعماريّات) ───
# أزواج (وصف, مسار_YAML_النسبي, مسار_Schema): كلّ جدول خلفيّة يُتحقَّق ضدّ مخطّطه.
# تُوسَّع مع كلّ معماريّة/جدول جديد (arm64/riscv64/… ، وأنظمة abi إضافيّة).
BACKEND_TABLE_PAIRS: list[tuple[str, str, str]] = [
    # x86_64 (عائلة variable) — الهدف المرجعيّ (م٠)
    ("x86_64 تعليمات", "backend/x86_64/instructions.yaml", "backend_encoding.schema.json"),
    ("x86_64 سجلّات",  "backend/x86_64/registers.yaml",    "backend_register_file.schema.json"),
    ("x86_64 اختيار",  "backend/x86_64/isel.yaml",         "backend_isel.schema.json"),
    ("x86_64-linux ABI", "backend/abi/x86_64-linux.yaml",  "backend_abi.schema.json"),
    # arm64 (عائلة fixed32) — الهدف الثاني: يُبرهِن عموميّة المخطّطات (RISC مقابل CISC)
    ("arm64 تعليمات",  "backend/arm64/instructions.yaml",  "backend_encoding.schema.json"),
    ("arm64 سجلّات",   "backend/arm64/registers.yaml",     "backend_register_file.schema.json"),
    ("arm64 اختيار",   "backend/arm64/isel.yaml",          "backend_isel.schema.json"),
    ("aarch64-linux ABI", "backend/abi/aarch64-linux.yaml", "backend_abi.schema.json"),
]


class TestBackendTables:
    """اختبارات T1 لطبقة الخلفيّة السياديّة — الجداول تُتحقَّق ضدّ مخطّطاتها."""

    def test_backend_dir_exists(self):
        """language-truth/backend/ موجود بهدفَي العائلتين (x86_64=variable، arm64=fixed32) وجذر abi."""
        backend = TRUTH_DIR / "backend"
        assert backend.is_dir(), "language-truth/backend/ غير موجود"
        assert (backend / "x86_64").is_dir(), "backend/x86_64/ غير موجود"
        assert (backend / "arm64").is_dir(), "backend/arm64/ غير موجود"
        assert (backend / "abi").is_dir(), "backend/abi/ غير موجود"

    def test_both_encoding_families_present(self):
        """الهدفان يغطّيان عائلتَي الترميز: x86_64=variable، arm64=fixed32 (إثبات عموميّة المخطّط)."""
        x86 = load_yaml(TRUTH_DIR / "backend/x86_64/instructions.yaml")
        arm = load_yaml(TRUTH_DIR / "backend/arm64/instructions.yaml")
        assert x86["encoding_family"] == "variable", "x86_64 يجب أن يكون variable"
        assert arm["encoding_family"] == "fixed32", "arm64 يجب أن يكون fixed32"

    @pytest.mark.parametrize("schema_name", [
        "backend_encoding.schema.json",
        "backend_register_file.schema.json",
        "backend_isel.schema.json",
        "backend_abi.schema.json",
    ])
    def test_backend_schema_exists_and_valid_json(self, schema_name: str):
        """كلّ مخطّط خلفيّة موجودٌ وهو JSON صالح بمعرّف $id."""
        path = SCHEMAS_DIR / schema_name
        assert path.exists(), f"مخطّط خلفيّة مفقود: {schema_name}"
        schema = load_json(path)
        assert isinstance(schema, dict), f"المخطّط ليس كائن JSON: {schema_name}"
        assert "$id" in schema, f"المخطّط يفتقر $id: {schema_name}"

    @pytest.mark.parametrize("desc,yaml_path,schema_name", BACKEND_TABLE_PAIRS)
    def test_backend_table_validates(self, desc: str, yaml_path: str, schema_name: str):
        """كلّ جدول خلفيّة (YAML) يتطابق مع مخطّطه (كائنٌ واحد لا قائمة)."""
        import jsonschema
        data = load_yaml(TRUTH_DIR / yaml_path)
        assert data is not None, f"جدول فارغ: {yaml_path}"
        schema = load_json(SCHEMAS_DIR / schema_name)
        jsonschema.validate(data, schema)

    def test_encoding_family_forbids_mixed_encode(self):
        """تشديد: عائلة variable ترفض حقولَ fixed32 (fields) داخل encode — والعكس."""
        import jsonschema
        schema = load_json(SCHEMAS_DIR / "backend_encoding.schema.json")
        # جدولٌ يعلن variable لكنّه يخلط حقلَ fixed32 (fields) في encode ⇒ يجب أن يُرفَض.
        mixed = {
            "version": "1.0", "architecture": "x86_64", "word_bits": 64,
            "encoding_family": "variable", "status": "experimental",
            "instructions": {
                "انقل": [{
                    "en": "mov", "form": "r64, r64",
                    "operands": [{"kind": "reg"}, {"kind": "reg"}],
                    "encode": {"opcode": [0x89], "fields": [{"name": "x", "hi": 0, "lo": 0}]}
                }]
            }
        }
        with pytest.raises(jsonschema.ValidationError):
            jsonschema.validate(mixed, schema)

    def test_encoding_family_fixed32_requires_fields(self):
        """تشديد: عائلة fixed32 تُلزم width+fields وترفض حقولَ variable (opcode)."""
        import jsonschema
        schema = load_json(SCHEMAS_DIR / "backend_encoding.schema.json")
        bad = {
            "version": "1.0", "architecture": "arm64", "word_bits": 64,
            "encoding_family": "fixed32", "status": "experimental",
            "instructions": {
                "اجمع": [{
                    "en": "add", "form": "x, x, x",
                    "operands": [{"kind": "reg"}],
                    "encode": {"opcode": [0x0B]}
                }]
            }
        }
        with pytest.raises(jsonschema.ValidationError):
            jsonschema.validate(bad, schema)
