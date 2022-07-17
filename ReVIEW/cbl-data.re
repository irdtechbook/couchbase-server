= Couchbase Liteデータベース

== ドキュメント構造

Couchbase Liteのデータの基本単位は@<em>{ドキュメント}であり、これはRDBの行またはレコードに相当します。

Couchbase Liteの内部で、ひとつのドキュメントは一意かつ不変の@<em>{ドキュメントID}(キー)と、ドキュメントIDにより一意に特定されるデータ(バリュー)、つまりキーとバリューのペアから構成されます。


=== ドキュメントID

ドキュメントIDはデータベース内で一意であり、ドキュメント作成後に変更することができません。
ドキュメントIDはユーザーが作成時に指定するか、あるいは(UUIDとして)自動的に生成することができます。

ドキュメントIDとして利用できる文字列には、以下の制約あります。

//blankline

 * スペースを含まないUTF-8文字列(「@<tt>{%}」、「@<tt>{/}」、「@<tt>{"}」、「@<tt>{_}」などの特殊文字を許容)
 * 最大250バイト


=== JSONドキュメント

Couchbase Lieteデータベースに格納されるドキュメントのデータモデル(Documents Data Model@<fn>{couchbase-lite-document})は、JSONの形式を取ります。

JSONオブジェクトは、名前と値のペアのコレクションであり、値は、数値、文字列、null、JSONオブジェクト、そしてこれらの値の配列といった、さまざまなデータ型を取ります。

この「名前」と「値」のペアは@<em>{プロパティー}と呼ばれます。
「名前」のことをプロパティー名、「値」をプロパティー値と呼ぶことがあります。
このように、プロパティーは、データ自体に加え、名前という属性情報を含むため、JSONドキュメントは複雑なデータ構造を自己組織化された方法で表すことができます。

以下は、このようなJSONオブジェクトの例です。

//emlist[][json]{
{
    "名前1": "値1",
    "名前2": [ "値2A", "値2B", "値2C" ]
    "名前3": {
        "名前3A" : "値3A",
        "名前3B" : 123,
        "名前3C" : [ 1, 2, 3 ]
    },
    "名前4": null,
    "名前5": [
        { "名前5A" : "値5A1", "名前5B" : 1 },
        { "名前5A" : "値5A2", "名前5B" : 2 },
        { "名前5A" : "値5A3", "名前5B" : 3 }
    ]
}
//}



//footnote[couchbase-lite-document][https://docs.couchbase.com/couchbase-lite/current/android/document.html]

=== 添付ファイル(バイナリデータ)

ドキュメントには、@<em>{添付ファイル}と呼ばれるバイナリデータ(BLOB)を関連付けることができます。
添付ファイルによって、Couchbase Liteデータベースは、画像ファイルやその他の非テキストデータを保存する手段を提供します。

添付ファイルは、添付先のドキュメントと同じCouchbase Liteデータベースインスタンスに保存されます。
添付ファイルは、内部的にはそのファイルが添付されたJSONドキュメントとは異なるドキュメントIDを持つ別のドキュメントとして保存されます。
添付先ドキュメントには、そのドキュメント自体のデータに加え、添付ファイルへの参照や添付ファイルの属性情報に関するデータが加えられます。

ひとつのドキュメントに対して、複数の添付ファイルを関連付けることができます。
反対に、同じ添付ファイルを複数のドキュメントに添付することもでき、その場合、添付ファイルのひとつのインスタンスのみがデータベースに保存されます。

=== ドキュメント有効期限

ドキュメントに対して有効期限を設定できます。
ドキュメントの有効期限が切れると、ドキュメントはデータベースからパージ(消去)されます。


== データベース操作

Couchbase Liteデータベース(Databases@<fn>{couchbase-lite-database})を利用する際の基本的な操作について解説します。


//footnote[couchbase-lite-database][https://docs.couchbase.com/couchbase-lite/current/java/database.html]


=== 初期化

最初に、@<tt>{Database}クラスの静的メソッド@<tt>{init()}をコールし、データベースを初期化します。

//emlist[][]{
CouchbaseLite.init(context);
//}

上の例で、引数として与えられているのは、@<tt>{android.content.Context}オブジェクトです。

初期化前に他のAPIが呼び出されると、例外が発生します。

=== データベース作成/オープン

@<tt>{Database}クラスを使用して、新しいデータベースを作成したり、既存のデータベースをオープンすることができます。
@<tt>{Database}クラスのインスタンスを作成する際には、データベース名を指定します。
また、必要に応じ、@<tt>{DatabaseConfiguration}オブジェクトを作成の上、@<tt>{Database}クラスのコンストラクターへの引数として指定します。

以下の例では、@<tt>{DatabaseConfiguration}を用いて、データベースのパスを指定しています。
パスを指定しない場合は、デフォルトの場所が利用されます。

//emlist[][java]{
final String DB_NAME = "CBL";

DatabaseConfiguration config = new DatabaseConfiguration();
config.setDirectory(context.getFilesDir().getAbsolutePath());

Database database = new Database(DB_NAME, config);
//}

指定された名前のデータベースが指定されたパスに存在しない場合、新しいデータベースが作成されます。

====[column]データベース暗号化

エンタープライズエディションでは、データベースを暗号化することができます。

暗号化を有効にするには、データベース作成時に@<tt>{DatabaseConfiguration}の@<tt>{encryptionKey}プロパティーに暗号化キーを設定します。
暗号化に使用されるアルゴリズムは256ビットAESです。

暗号化されたデータベースをオープンする際には、複合化のために暗号化キーを指定する必要があります。

暗号化キーの管理は、アプリケーション側の責任になります。
暗号化キーは通常、AppleのキーチェーンやAndroidのキーストアなどのプラットフォーム固有の安全なキーストアに保存されます。

====[/column]

=== データベースクローズ


以下は、データベースをクローズする例です。

//emlist[][java]{
database.close();
//}



== ドキュメント操作


=== ドキュメント作成・保存

新規ドキュメント作成時には、@<tt>{MutableDocument}クラスを使用します。次のいずれかのコンストラクターを使用できます。

//blankline

 * @<tt>{MutableDocument(String id)}: 指定したドキュメントIDを使用して新しいドキュメントを作成します。
 * @<tt>{MutableDocument()}: ドキュメントIDをランダムに生成して、新しいドキュメントを作成します。

//blankline

次のコードは、ドキュメントを作成してデータベースに永続化する例です。


//emlist[][JAVA]{
MutableDocument newTask = new MutableDocument();
newTask.setString("type", "task");
newTask.setDate("createdAt", new Date());
try {
    database.save(newTask);
} catch (CouchbaseLiteException e) {
    Log.e(TAG, e.toString());
}
//}

=== ドキュメント取得

ドキュメントIDを引数として取る、@<tt>{Database}クラスの@<tt>{getDocument}メソッドを使用して、ドキュメントを取得することができます。

//emlist[][JAVA]{
Document document = database.getDocument("xyz");
//}

指定されたドキュメントIDを持つドキュメントが存在しない場合は、nullが返されます。


=== ドキュメント変更

データベースからドキュメントが読み取られるとき、ドキュメントは不変(immutable)オブジェクトとして取得されます。
ドキュメントを更新する際には、@<tt>{toMutable}メソッドを使用して、更新可能な(mutable)インスタンスを取得します。

ドキュメントへの変更は、@<tt>{Database}の@<tt>{save}メソッドコールのタイミングでデータベースに反映されます。


//emlist[][JAVA]{
Document document = database.getDocument("xyz");
MutableDocument mutableDocument = document.toMutable();
mutableDocument.setString("name", "apples");
try {
    database.save(mutableDocument);
} catch (CouchbaseLiteException e) {
    Log.e(TAG, e.toString());
}
//}

=== 型付きアクセサー


@<tt>{Document}クラスは、文字列、数値(整数、浮動小数点数)、ブーリアン(真偽値)など、さまざまな型のプロパティーへのアクセサーを提供します。
これらのアクセサーを利用して、アプリケーションが期待する型とJSONエンコーディングとの間の変換を行うことができます。

アクセサーの引数として指定された名前のプロパティーがドキュメント内に存在しない場合、アクセサーのタイプに応じたデフォルト値が返されます（たとえば、整数値に対するアクセサー@<tt>{getInt}の場合は@<tt>{0}のように）。

=== Dateアクセサー

@<tt>{Date}型は一般的なデータ型ですが、JSONはネイティブにサポートしていないため、ISO-8601形式の文字列として日付データを格納するのが慣例です。
@<tt>{Document}クラスは、@<tt>{Date}型アクセサーを備えています。

次の例では、@<tt>{java.util.Date}オブジェクトを@<tt>{MutableDocument}のプロパティーとして設定しています。


//emlist[][JAVA]{
mutableDocument.setValue("createdAt", new Date());
//}

次の例では、@<tt>{Document}オブジェクトの@<tt>{getDate}アクセサーを使用して@<tt>{Date}オブジェクトとして取得しています。

//emlist[][JAVA]{
Date date = document.getDate("createdAt");
//}


=== 配列操作

@<tt>{Document}クラスは、配列型のプロパティーへのアクセサーを持ちます。

@<tt>{Document}の配列型のプロパティー値として、Couchbase Liteは@<tt>{Array}クラスと@<tt>{MutableArray}クラスを提供します。


以下は、@<tt>{MutableArray}を@<tt>{Document}のプロパティーとして追加する例です。

//emlist[][JAVA]{
// MutableArrayオブジェクト作成
MutableArray mutableArray = new MutableArray();

// 要素の追加
mutableArray.addString("650-000-0000");
mutableArray.addString("650-000-0001");

// 新規ドキュメントのプロパティーとしてMutableArrayオブジェクトを追加
MutableDocument mutableDoc = new MutableDocument("doc1");
mutableDoc.setArray("phones", mutableArray);

// ドキュメント保存
database.save(mutableDoc);
//}

以下は、@<tt>{Document}から@<tt>{Array}を取得して利用する例です。

//emlist[][JAVA]{
Document document = database.getDocument("doc1");

// ドキュメントプロパティーから配列を取得
Array array = document.getArray("phones");

// 配列の要素数をカウント
int count = array.count();

// インデックスによる配列アクセス
for (int i = 0; i < count; i++) {
    Log.i(TAG, array.getString(i));
}

// ミュータブルコピーの生成
 MutableArray mutableArray = array.toMutable();
//}


=== ディクショナリー操作

@<tt>{Document}クラスは、ディクショナリー型のプロパティーへのアクセサーを持ちます。

@<tt>{Document}のディクショナリー型のプロパティー値として、Couchbase Liteは@<tt>{Dictionary}クラスと@<tt>{MutableDictionary}クラスを提供します。

以下は、@<tt>{MutableDictionary}を@<tt>{Document}のプロパティーとして追加し、データベースに保存する例です。

//emlist[][JAVA]{
// MutableDictionaryオブジェクト作成
MutableDictionary mutableDict = new MutableDictionary();

// ディクショナリーへのキー/値の追加
mutableDict.setString("street", "1 Main st.");
mutableDict.setString("city", "San Francisco");

// 新規ドキュメントのプロパティーとしてMutableDocumentオブジェクトを追加
MutableDocument mutableDoc = new MutableDocument("doc1");
mutableDoc.setDictionary("address", mutableDict);

// ドキュメント保存
database.save(mutableDoc);
//}

以下は、@<tt>{Document}から@<tt>{Dictionary}を取得して利用する例です。

//emlist[][JAVA]{
Document document = database.getDocument("doc1");

// ドキュメントプロパティーからディクショナリーを取得
Dictionary dict = document.getDictionary("address");

// キーによる値の取得
String street = dict.getString("street");

// ディクショナリーに対する走査
for (String key : dict) {
    Log.i(TAG, key + ":" + dict.getValue(key));
}

// ミュータブルコピーの生成
MutableDictionary mutableDict = dict.toMutable();
//}

=== プロパティー確認

ドキュメントに特定のプロパティーが存在するかどうかを確認するために、@<tt>{contains}メソッドを使用できます。


//emlist[][JAVA]{
Document document = database.getDocument("doc1");
String key = "key1";
if (document.contains(key)) {
    Log.i(TAG, key + ":" + document.getString(key));
}
//}

=== バッチ操作

データベースに一度に複数の変更を加える場合、それらをグループ化してバッチとして実行する方法が提供されています。

次の例では、@<tt>{inBatch}メソッドに与えるコードブロックの中で、ドキュメントをデータベースに保存する処理を複数回実行しています。

//emlist[][JAVA]{
database.inBatch(() -> {
    for (int i = 0; i < 10; i++) {
        MutableDocument doc = new MutableDocument();
        doc.setValue("type", "user");
        doc.setValue("name", "user " + i);
        doc.setBoolean("admin", false);
        try {
            database.save(doc);
			Log.i(TAG, String.format("saved user document %s", doc.getString("name")));
        } catch (CouchbaseLiteException e) {
            Log.e(TAG, e.toString());
        } 
    }
});
//}

バッチとして実現される一連の操作は、ローカルレベルでトランザクショナルです。
つまり、他のプロセスは、コードブロック実行中に変更を加えることができず、また、コードブロック中の変更の一部を認識するということがありません。

ローカルレベルと断っているのは、Couchbase LiteとCouchbase Serverとのデータ同期を行う場合、これらのグループ化された操作によるCouchbase Liteデータベースに対する更新が、Couchbase Serverに対して反映される際に、トランザクショナルに実施されるわけではない、という意味合いです。


=== 変更イベントのリスニング

データベースやドキュメントで発生した変更をイベントとして検知することによって、要件に応じた処理を実装することが可能です。

次の例では、ドキュメントに対する変更を検知するために、特定のドキュメントに対してリスナーを登録しています。


//emlist[][]{
database.addDocumentChangeListener(
    "doc1",
    change -> {
        Document doc = database.getDocument(change.getDocumentID());
        if (doc != null) {
            Toast.makeText(context, "Status: " + doc.getString("status"), Toast.LENGTH_SHORT).show();
        }
    });
//}



=== ドキュメント有効期限設定


ドキュメントに有効期限を設定できます。ドキュメントの有効期限が切れると、ドキュメントはデータベースからパージ(消去)されます。

次の例では、ドキュメントのTTL(Time To Live)を現在の時刻から1日後に設定しています。

//emlist[][JAVA]{
Instant ttl = Instant.now().plus(1, ChronoUnit.DAYS);
database.setDocumentExpiration("doc1", new Date(ttl.toEpochMilli()));
//}

以下のように、nullを設定することで、有効期限をリセットすることができます。

//emlist[][JAVA]{
database.setDocumentExpiration("doc1", null);
//}


== 添付ファイル操作

=== 添付ファイル作成

以下は、ドキュメントの添付ファイルを作成し、データベースに保存する例です。

//emlist[][]{
AssetManager assetManager = context.getResources().getAssets();
InputStream is = null;
try {
    is = assetManager.open("image.jpg");
    Blob blob = new Blob("image/jpeg", is);
    mutableDoc.setBlob("image", blob);
    database.save(mutableDoc);
} catch (IOException e) {
    Log.e(TAG, e.toString());
} catch (CouchbaseLiteException e) {
    Log.e(TAG, e.toString());
} finally {
    try {
        if (is != null) {
            is.close();
        }
    } catch (IOException ignore) { }
}
//}

=== 添付ファイル取得

以下は、ドキュメントから添付ファイルを取得する例です。

//emlist[][]{
Blob blob = doc.getBlob("image");
byte[] bytes = blob.getContent();
//}


== JSON文字列との変換

Couchbase Liteの@<tt>{Document}、@<tt>{MutableDocument}、@<tt>{Dictionary}、@<tt>{MutableDictionary}、@<tt>{Array}、@<tt>{MutableArray}クラスは、JSON文字列との変換をサポートしています。


=== ドキュメントからJSON文字列への変換

@<tt>{Document}クラスの@<tt>{toJSON()}メソッドを使用して、ドキュメントの内容をJSON文字列として取り出すことができます。


//emlist[][java]{
final String json = doc.toJSON(); 
//}

=== JSON文字列によるドキュメント作成

JSON文字列を使用して@<tt>{MutableDocument}オブジェクトを作成することができます。

次の例では、まずデータベースから取得したドキュメントをJSON文字列に変換し、そのJSON文字列を使用して別のドキュメントとして保存しています。

//emlist[][Java]{
final String json = database.getDocument("doc1").toJSON();

final MutableDocument document = new MutableDocument("doc2", json);

database.save(document);
//}


=== JSON文字列とディクショナリーとの変換

JSON文字列を使用して@<tt>{MutableDictionary}オブジェクトを作成することができます。

以下は、JSON文字列とディクショナリーの変換を扱う例です

//emlist[][Java]{
final String JSON = "{\"name1\":\"value1\",\"name2\":\"value2\"}";
final MutableDictionary mDict = new MutableDictionary(JSON);

for (String key: mDict.getKeys()) {
  Log.i(TAG, key + ":" + mDict.getValue(key));
}
//}

=== JSON文字列と配列との変換

JSON文字列を使用して、@<tt>{MutableArray}オブジェクトを作成することができます。

以下は、JSON文字列と配列の変換を扱う例です。

//emlist[][Java]{
final String JSON = "[{\"id\":\"obj1\"},{\"id\":\"obj2\"}]";
final MutableArray mArray = new MutableArray(JSON);

for (int i = 0; i < mArray.count(); i++) {
    final Dictionary dict = mArray.getDictionary(i);
    Log.i(TAG, dict.getString("name"));
    db.save(new MutableDocument(dict.getString("id"), dict.toMap()));
}
//}


=== 開発参考情報

ここでは、Couchbase Lite APIによるJSON操作について解説しました。

一旦、Couchbase LiteオブジェクトからJSONデータが文字列として取り出された後は、プログラミング言語で提供されているJSON用ライブラリーを活用することができます。
たとえば、ライブラリーの提供するデータバインディング機能を利用して、JSONからPOJO(Plain Old Java Object)に変換することが考えられます。

そのようなライブラリーの例として、Jackson@<fn>{github-jackson}があります。
Couchbase LiteとJacksonの組み合わせは、Couchbase Blog: Exploring Couchbase Mobile on Android: Object Mapping@<fn>{object-mapping-couchbase-mobile-android}でも取り上げられています。

//footnote[github-jackson][https://github.com/FasterXML/jackson]
//footnote[object-mapping-couchbase-mobile-android][https://blog.couchbase.com/object-mapping-couchbase-mobile-android/]

