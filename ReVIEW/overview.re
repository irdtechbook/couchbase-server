= なぜ、Couchbase Mobileなのか?

この章では、Couchbase Mobileの存在意義と、その技術的位置づけについて解説します。

== モバイル/エッジコンピューティングデータプラットフォームCouchbase Mobile

=== はじめに

Couchbaseという名前を持つデータベースには、Couchbase ServerとCouchbase Liteのふたつが存在します。
モバイル/エッジコンピューティングデータプラットフォームという観点において、これらCouchbase ServerとCouchbase Liteの両方が重要な意味を持ちます。

「Couchbase Mobile」とは、単体のデータベースないし何らかのソフトウェアを指す言葉ではなく、Couchbase LiteとSync Gatewayを包含する呼称です。

Couchbase Liteは、AndroidやiOS、またはエッジデバイス上で実行されるアプリケーション用の組込データベース(Embeded Database@<fn>{wiki-Embedded_database})です。
Sync Gatewayは、Couchbase LiteとCouchbase Serverとのデータ同期を行う役割を担います。
具体的には、Couchbase Serverと共に用いられるサーバーソフトウェアです。

Couchbase Liteは、それ単体のみで利用することもできますが、ここでは、モバイル/エッジコンピューティングデータプラットフォームとしてのCouchbase Mobile総体について、「なぜCouchbase Mobileなのか?」という問いに答える観点から整理します。

//footnote[wiki-Embedded_database][https://en.wikipedia.org/wiki/Embedded_database]

=== モバイルアプリケーションにおけるCouchbase Mobileの価値

モバイルアプリケーションで利用されるデータについては、モバイル端末上で作成・保存・利用されれば十分ということはむしろ稀であり、サーバーからのデータ取得やサーバーへのデータ登録・更新が行われることは珍しくありません。
そのため、モバイルアプリケーション開発においては、多くの場合、アプリケーション内部のデータ管理と、サーバーとのデータ通信、という二つの技術的要件が存在します。
Couchbase Mobileは、ローカルデータベース(Couchbase Lite)とリモートデータベース(Couchbase Server)間の双方向の自動データ同期機能の提供という形で、この必要性に応えます。

Couchbase Mobileでは、データ同期処理のことを@<tt>{レプリケーション}、その処理を実行するモジュールを@<tt>{レプリケーター}と呼びます。
「レプリケーション(Replication)」という語自体は一般用語であり、コンピューティングの分野で用いられている場合も文脈に応じて多様な意味を持ちますが、Couchbase Mobileの文脈で単にレプリケーションという場合には、上述の内容を指しています。

「サーバーとのデータ通信」という観点において、ローカルデータベースとリモートデータベースとの間のデータ同期(レプリケーション)という手法は、選択肢の一つということは言えますが、決して王道とはいえないでしょう。
クライアントアプリケーションとサーバーとの間でデータ通信が行われる場合、ローカルにデータベースを持つことは冗長ないし不要、さらにいえば、サーバーにあるデータこそがシングル・ソース・オブ・トゥルース(Single Source of Truth)であり、ローカルデータベースの存在は、データの一元管理という原則に反する、という観点もあり得るかと思います。
これは、特にWebアプリケーション開発に馴染んだ立場からは自然な発想ともいえるでしょう。
このような見方を踏まえた上でなお、ローカルデータベースを持つ理由・利点として、以下があります。

//blankline

 * ネットワーク依存の分離
 * 通信ロジックの分離

//blankline

以下、それぞれについて具体的に見ていきます。


=== ネットワーク依存の分離


ローカルデータベースを持つことによって、ネットワーク通信が行えない環境下ではローカルに保存してあるデータを利用しつつ、ネットワークが回復した際にデータの同期を行うことができます。
例えば、想像しやすいところでは、飛行機の搭乗員用のアプリケーションを考えてみることができます。@<fn>{customers-united-airlines}
このような設計を指して@<strong>{オフラインファースト}という言葉が使われます。

一方で、昨今では「ネットワーク通信が行えない環境」と言われても、非常に特殊なケースという印象を持つ方も多いかもしれません。
その意味では、オフラインファーストアプリケーションは、@<strong>{ユーザーエクスペリエンス向上}の一形態と考えた方が分かりやすいかもしれません。


//footnote[customers-united-airlines][https://www.couchbase.com/customers/united-airlines]


ローカルデータベースを持たないアプリケーションが、サーバーからデータを取得して表示する場合、ユーザーがアプリケーションを立ち上げてから、データが画面に表示されるまでの時間は、たとえそれが短い時間であっても、ユーザー体験としては意味のない単なる待ち時間でしかありません。
これは旧来のWebアプリケーションに典型的に見られる状況ともいえます。

ローカルデータベースは、このような形での「ネットワーク依存」をアプリケーションの操作性から分離するために役立ちます。
ローカルデータベースを用いることによって、アプリケーションの起動直後はローカルに保存されているデータを利用しながら、データの最新化については、バックグラウンドでユーザーが気がつかないうちに行うという設計が容易に実現可能になります。
当然ながら、これはローカルデータベースを導入するだけで自動的に解決するものではなく、開発者が適切にユーザー体験をデザインする必要があります。


=== 通信ロジックの分離

ここまで、ローカルデータベースの意義について、ユーザーの利便性の観点から見てきました。「オフラインファースト」にしろ「ユーザーエクスペリエンス向上」にしろ、必要なのはデータの扱い方を適切に設計することであり、ローカルデータベースの導入は、その手段でしかないといえます。

ここで、「ローカル」データベースに関する考察を離れて、より広い視野からデータベース一般に視点を移動すると、そもそもデータベースは、システムにおけるデータ(管理)と(ビジネス)ロジックの分離のために存在しているといえます。
つまり、データ管理に必要な汎用的な処理を信頼性のある既存のテクノロジーに任せることによって、開発者は、それ以外の部分の開発に注力することができるようになります。
ローカルデータとリモートデータの同期を、データベース（データプラットフォーム）に任せることによって、開発者はさらに、通信ロジックの開発についても、信頼性のある既存のテクノロジーに委ねることが可能になります。

これには、次のような副産物が伴います。

=== データ操作性の一元化

ローカルデータとリモートデータが暗黙理に同期される状況において、開発者は、ローカルで生成されるデータとリモートから入手するデータについて、それぞれ異なる取り扱い方をする必要がなくなります。
アプリケーションにおけるデータ操作は、データがローカルで生成・保存されたのか、リモートから同期されたのかどうかに関わらず、ローカルデータベースに対するAPIコールやクエリという形でいわば一元化されます。

=== データへのアプローチのパラダイムシフト

この場合、開発者は、リモートデータの入手や保存のための「操作」を設計・開発するのではなく、同期されるデータの範囲や利用できるユーザーやロールを、プラットフォームの提供する方法によって、「構成」することになります。
これは、命令的プログラミング(Imperative programming@<fn>{wiki-Imperative_programming})から宣言的プログラミング(Declarative programming@<fn>{wiki-Declarative_programming})へのパラダイムシフトに比して考えることができます。
あるいは、Kubernetes@<fn>{wiki-Kubernetes}における宣言的構成管理との類比で捉えることもできるでしょう。

//footnote[wiki-Imperative_programming][https://en.wikipedia.org/wiki/Imperative_programming]
//footnote[wiki-Declarative_programming][https://en.wikipedia.org/wiki/Declarative_programming]
//footnote[wiki-Kubernetes][https://en.wikipedia.org/wiki/Kubernetes]

=== マルチチャネルにおけるテクノロジー基盤

また現在では、一つのサービスが、モバイルアプリケーションとWebアプリケーションの両方の形態で提供されることも珍しくありません。
このような場合には、クラウドやデータセンターに存在するデータベースが、ふたつの異なる形態で実装されたアプリケーションのための共通のデータソースとなります。
共通のデータソースとなるデータベースに、ローカルデータベースとの同期をサポートしているデータベースを採用することは、サービスを構成するシステム全体にとって、統一されたテクノロジー基盤を持つことに繋がります。


=== 開発コスト削減と先進的技術導入促進

通信ロジックを個々のアプリケーションのために新たに開発しなくても済むことが開発コストの削減につながることは見やすいところです。
また、既存の技術を活用することは、自ら開発するよりも、すでに多くの環境で実績のある、より信頼性の高いコードを利用することにつながります。
もっとも、学習コストやメンテナンス性の違いなど考慮すべき要素があり、単に楽観的にコストが削減できると考えるのは短絡的であるという見方もあるかもしれません。

一方、既存の技術を活用することによって、先進的な技術の導入のハードルを下げることができるという利点があります。
以下、この観点から、技術的な内容に及びますが、例を挙げて説明してみたいと思います。

Couchbase Mobileのレプリケーションは、WebSocket上のメッセージングプロトコルとして実装されています。
WebSocketプロトコルは、単一のTCPソケット接続を介してリモートホスト間で全二重メッセージを渡すことを可能にします。
Couchbase Mobileのメッセージングプロトコルは、WebSocketレイヤーを用いた階層化アーキテクチャにより、レプリケーションロジックとその基盤となるメッセージングトランスポートの間の「関心の分離」を実現しています。

WebSocketプロトコルは、REST APIのようなHTTPをベースとしたプロトコルよりも高速で、必要となる帯域幅とソケットリソースを削減することができます。
さらに、ソケットリソースの節約により、サーバー側での同時接続数を増やすことができます。
このようなWebSocketによるメッセージプロトコルを、自ら開発するのは相当に高いハードルだといえます。
多くの場合、WebSocketを用いることは絶対条件ではないかもしれませんが、その利点は、既存技術を利用することによる導入の容易さと併せて、検討する価値があると考えられます。

上記のような要素技術の面だけでなく、より一般的にいって、特にエンタープライズでの利用において求められるレベルのセキュリティを考慮して設計・実装され、多くの運用実績を持つ環境を利用できるという利点は、付け加えておく意味があるでしょう。

これらは一部の例ですが、コスト最適化は、目先の開発コスト削減に関してのみではなく、サービスの先進性や品質等に関する競合との差別化に要するコストと効果のバランスの面からも見る必要があることがわかります。


//footnote[database-encryption][https://docs.couchbase.com/couchbase-lite/current/android/database.html#database-encryption]

//footnote[openid-connect][https://openid.net/connect/]

//footnote[doc-openid-connect][https://docs.couchbase.com/sync-gateway/current/authentication-users.html#openid-connect]


=== エッジコンピューティングにおけるCouchbase Mobileの価値


//footnote[wiki-Mobile_computing][https://en.wikipedia.org/wiki/Mobile_computing]

//footnote[wiki-Edge_computing][https://en.wikipedia.org/wiki/Edge_computing]


エッジコンピューティングにおける重要な要素としてエッジデータセンターの活用があります。
各パブリッククラウドから、下記のようなエッジデータセンターを実現するサービスが提供されています。

//blankline

 * AWS Local Zones
 * AWS Wavelength
 * AWS Outpost
 * Azure Edge Services
 * Google Distributed Cloud Powered by Anthos

//blankline

エッジデータセンターの活用、すなわち複数の異なるデータセンターからなる多階層でのデータ同期においては、きめ細かいアクセス制御を実施しながら、中央集中型データセンターと多数のエッジデータセンター間でのデータ同期を制御する必要があります。
上述のようなサービスが各社から提供され利用できる現在の状況を鑑みて、このような要件は、ますます重要になっているといえるでしょう。

Couchbase Mobileは、サーバーに対する複数のクライアントからなる古典的なスタートポロジー構成に留まらず、マルチティアのネットワーク構成に対応することができます。

Sync Gatewayには、Sync Gateway間レプリケーション(Inter Sync Gateway Replication)機能が備わっています。これによって、複数の異なるデータセンターに存在するSync Gateway間の同期を実現します。

Sync Gateway間レプリケーションを用いることによって、クラウドや中央データセンターにあるCouchbase Serverと同期する、ネットワークトポロジーのハブとしてCouchbase ServerとSync Gatewayをエッジデータセンターに配置することができます。これにより、Couchbase Liteを使ったアプリケーションは、高速・低遅延といった、エッジデータセンターの提供するデータローカリティーによる恩恵を受けることができます。

=== エッジデバイスにおけるCouchbase Mobileの価値

センサー等のIoT/エッジデバイスで発生した情報を収集するための技術として、データストリーミングを実現する様々な技術が存在します。
エッジデバイスからデータ収集を行う際の構成要素として、オープンソーステクノロジーとしては、例えば以下のようなものがあります。

//blankline

 * Apache NiFi/MiNiFi
 * Apache Flink
 * Apache Kafka
 * Apache Spark Streaming

//blankline

このようなデータストリーミングの技術を組み合わせ、下流と上流でデータの入出力を構成する代わりに、エッジデバイス上のデータベースとしてCouchbase Liteを配した上で、通信に関わる処理をCouchbase Mobileに委ねることが考えられます。

Kafkaのようなソフトウェアは、ストリーミングデータをキューで管理することによって内部にデータを持つため、ネットワークの信頼性の影響を抑えることができますが、ローカルデータベースを活用することによって、オフライン状況でのデータハンドリングがより直接的に容易になる、という捉え方もできます。

Couchbase Liteは、AndroidやiOSのようなモバイル端末で利用することができるだけではありません。
Android JavaのみでなくJVMが実行できる環境で利用することができる他、C言語サポートが提供されており、Raspberry Piや、Debian、UbuntuのようなLinux OSが用いられているエッジデバイス上で、ネイティブアプリケーションに組み込んで利用することができます。


== ドキュメント指向組み込みデータベースCouchbase Lite

=== はじめに

ここでは、Couchbase Liteデータベースが、他のモバイル組み込みデータベース技術の中で、どのように位置づけられるかを見ていきます。

モバイルアプリケーション用の組み込みデータベースの中でも、よく知られたものとして、SQLite@<fn>{sqlite-org}があります。
あるいは、iOS開発者であればCore Data@<fn>{690437fb48449bb9f6d77a28ed994110}を、Android開発者の場合はRoom@<fn>{developer-android-room}のことを考えるかもしれません。
また、それらのようなプラットフォーム固有ではない技術として、Realm@<fn>{fd21bfd2f123ff52f02fec57f480fa2e}が知られています。
ここでは、これらについて順に見ていきます。
その際には、データベースの特徴を最も端的に表す要素として、データモデリングの観点に特に注目して解説します。

//footnote[sqlite-org][https://www.sqlite.org/index.html]

//footnote[690437fb48449bb9f6d77a28ed994110][https://developer.apple.com/documentation/coredata]

//footnote[developer-android-room][https://developer.android.com/jetpack/androidx/releases/room]

//footnote[fd21bfd2f123ff52f02fec57f480fa2e][https://realm.io/]

=== SQLite


SQLiteは、「SQLデータベースエンジンを実装するC言語ライブラリであり、世界で最も利用されているデータベースエンジン」です(括弧内は、公式サイトからの抄訳)。

以下に、Android developerサイトの公式ドキュメント@<fn>{developer-android-training-sqlite}からのコードを引用しながら、基本的な使い方を紹介します。

まずは、定数定義(Contract Class)を見てみます。

//emlist[][]{
public final class FeedReaderContract {
    // To prevent someone from accidentally instantiating the contract class,
    // make the constructor private.
    private FeedReaderContract() {}

    /* Inner class that defines the table contents */
    public static class FeedEntry implements BaseColumns {
        public static final String TABLE_NAME = "entry";
        public static final String COLUMN_NAME_TITLE = "title";
        public static final String COLUMN_NAME_SUBTITLE = "subtitle";
    }
}
//}

データを格納するためには、テーブル定義を行います。
以下は、そのためのクエリの例です。


//emlist[][]{
private static final String SQL_CREATE_ENTRIES =
    "CREATE TABLE " + FeedEntry.TABLE_NAME + " (" +
    FeedEntry._ID + " INTEGER PRIMARY KEY," +
    FeedEntry.COLUMN_NAME_TITLE + " TEXT," +
    FeedEntry.COLUMN_NAME_SUBTITLE + " TEXT)";

private static final String SQL_DELETE_ENTRIES =
    "DROP TABLE IF EXISTS " + FeedEntry.TABLE_NAME;
//}


以下が、データベースにデータを挿入するためのコードです。
先に確認したテーブル定義に従って、インサートするレコードの構造が決定されます。

//emlist[][]{
// Gets the data repository in write mode
SQLiteDatabase db = dbHelper.getWritableDatabase();

// Create a new map of values, where column names are the keys
ContentValues values = new ContentValues();
values.put(FeedEntry.COLUMN_NAME_TITLE, title);
values.put(FeedEntry.COLUMN_NAME_SUBTITLE, subtitle);

// Insert the new row, returning the primary key value of the new row
long newRowId = db.insert(FeedEntry.TABLE_NAME, null, values);
//}

//footnote[developer-android-training-sqlite][https://developer.android.com/training/data-storage/sqlite]

以上、SQLiteの利用方法を見てきました。
リレーショナルデータベースを用いたアプリケーション開発経験者にとっては、特に多くを説明する必要のない内容であると思います。


SQLiteについて、同じくAndroidの公式ドキュメントでは、以下のように問題点が指摘されています。

//quote{
RAW SQLクエリはコンパイル時に検証されません。
//}

//quote{
SQLクエリとデータオブジェクトを変換するには、大量のボイラープレートコードを記述する必要があります。
//}

そして、下記のように注意されています。

//quote{
SQLiteデータベース内の情報にアクセスするための抽象化レイヤとしてRoom永続ライブラリを使用することを強くおすすめします。
//}


=== Room

Roomは、Android SDKが提供するORM(Object Relational Mapping@<fn>{wiki-orm})フレームワークです。

Roomについても、SQLiteと同じく、Android developerサイトの公式ドキュメント@<fn>{developer-android-training-room}からコードを引用しながら、解説します。

//footnote[wiki-orm][https://en.wikipedia.org/wiki/Object%E2%80%93relational_mapping]
//footnote[developer-android-training-room][https://developer.android.com/training/data-storage/room?hl=ja#java]

以下は、Roomにおけるエンティティ定義の例です。

//emlist[][]{
@Entity
public class User {
    @PrimaryKey
    public int uid;

    @ColumnInfo(name = "first_name")
    public String firstName;

    @ColumnInfo(name = "last_name")
    public String lastName;
}
//}

以下は、上記エンティティに対応するDAO(Data Access Object)定義です。
内部で、SQLクエリが用いられているのが分かります。

//emlist[][]{
@Dao
public interface UserDao {
    @Query("SELECT * FROM user")
    List<User> getAll();

    @Query("SELECT * FROM user WHERE uid IN (:userIds)")
    List<User> loadAllByIds(int[] userIds);

    @Query("SELECT * FROM user WHERE first_name LIKE :first AND " +
           "last_name LIKE :last LIMIT 1")
    User findByName(String first, String last);

    @Insert
    void insertAll(User... users);

    @Delete
    void delete(User user);
}
//}

以下は、上記のDAOを扱う@<tt>{RoomDatabase}継承クラスです。

//emlist[][]{
@Database(entities = {User.class}, version = 1)
public abstract class AppDatabase extends RoomDatabase {
    public abstract UserDao userDao();
}
//}

次のコードでは、@<tt>{RoomDatabase}継承クラスのインスタンスを取得しています。

//emlist[][]{
AppDatabase db = Room.databaseBuilder(getApplicationContext(),
        AppDatabase.class, "database-name").build();
//}

以下では、DAOを介してエンティティのリストを取得しています。

//emlist[][]{
UserDao userDao = db.userDao();
List<User> users = userDao.getAll();
//}

上記の流れは、ORM(Object Relational Mapping)フレームワークに典型的なものといえるでしょう。

=== Core Data

Core Dataは、「アプリケーションでモデルレイヤーオブジェクトを管理するためのフレームワーク(a framework that you use to manage the model layer objects in your application)」です(括弧内の引用は、Apple公式ドキュメント「What Is Core Data?」@<fn>{developer-apple-coredata}より)。

//footnote[developer-apple-coredata][https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/CoreData/index.html]


Core Dateにおいて、データモデル定義には、Xcode IDEのモデルエディターUIを用います。

定義したモデルは、プロジェクト内で、マネージドオブジェクトとして利用することができます。

Core Dataについて、単にORM(Object Relational Mapping)フレームワークであるとする説明は誤りになるでしょうが、目的や開発された背景にはORMと共通のものを認めることができます。


=== Realm

Realmは、オブジェクト指向データベースです。

//footnote[realm-io][https://realm.io/]

//footnote[mongodb-realm][https://www.mongodb.com/realm]

Realmでは以下のように、データモデルを@<tt>{RealmObject}を継承したクラスとして定義します(コードは、公式ドキュメントQuick Start@<fn>{realm-sdk-android-quick-start-local}から引用）。
ここでは、@<tt>{Task}クラスを定義しています。


//emlist[][]{
import io.realm.RealmObject;
import io.realm.annotations.PrimaryKey;
import io.realm.annotations.Required;
public class Task extends RealmObject {
    @PrimaryKey private String name;
    @Required private String status = TaskStatus.Open.name();
    public void setStatus(TaskStatus status) { this.status = status.name(); }
    public String getStatus() { return this.status; }
    public String getName() { return name; }
    public void setName(String name) { this.name = name; }
    public Task(String _name) { this.name = _name; }
    public Task() {}
}
//}

以下は、データ挿入の例です。
@<tt>{Task}オブジェクトを直接、データベースに挿入します。

//emlist[][]{
Task Task = new Task("New Task");
backgroundThreadRealm.executeTransaction (transactionRealm -> {
    transactionRealm.insert(Task);
});
//}

以下は、データ検索の例です。
検索結果は、@<tt>{Task}として型付けされています。

//emlist[][]{
// all Tasks in the realm
RealmResults<Task> Tasks = backgroundThreadRealm.where(Task.class).findAll();
//}

//footnote[realm-sdk-android-quick-start-local][https://docs.mongodb.com/realm/sdk/android/quick-start-local/]

以上から分かる通り、Realmは、あらかじめデータモデルの定義が必要なオブジェクト指向データベースであって、MongoDBやCouchbase Serverのようなドキュメント指向データベースとは異なります。
MongoDB Atlasは、Realとのデータ同期に対応していますが、Realmとの同期の場合には、MongoDBクライアントと同等の柔軟性が提供されるわけではないことに注意が必要です。

Realmがオブジェクト指向データベースであることは単なる特徴であって、ドキュメント指向データベースよりも劣る欠点とは言えませんが、MongoDBとの関わりにおいて、それぞれの違いを意識する必要があります。

=== Couchbase Lite

Couchbase Liteは、ドキュメント指向データベースです。

Couchbase Liteでは、以下のようにドキュメントオブジェクト(@<tt>{MutableDocument})に対して、プロパティとして値を設定します。
事前にデータモデルを定義する必要はありません。

//emlist[][]{
MutableDocument newTask = new MutableDocument();
newTask.setString("type", "task");
newTask.setString("name", "データベース設計");
newTask.setString("owner", "TBD");
newTask.setString("status", "NEW");
newTask.setDate("createdAt", new Date());
try {
    database.save(newTask);
} catch (CouchbaseLiteException e) {
    Log.e(TAG, e.toString());
}
//}

このように、Couchbase Liteには、特定のデータモデルを表すテーブルスキーマやクラスといったものが存在しません。@<fn>{memo-cbs-type}

Couchbase Liteでは、データ(ドキュメント)の種類を表す以下の二つの方法があります。これらは時に併用されます。

//blankline

 * 種類を表す特定のプロパティを用います。典型的には「type」というプロパティ名が用いられます。
 * 種類を表す名称をドキュメントを一意に特定するキー(ドキュメントID)の一部に用います。例えば、「user::001」のように、種類を表す接頭辞の後、任意のセパレーターが置かれ、後続の値によりキーがデータベース内で一意となるように設計します。

//blankline

Couchbase Liteは、SQL準拠のクエリが利用できるため、例えば以下のようなWHERE句を用いて、特定の種類のドキュメントを検索することができます。@<fn>{memo-cbs-sql}

//emlist[][]{
WHERE type = 'task'
//}


//footnote[memo-cbs-type][Couchbase Serverでは、リレーショナルデータベースにおけるテーブルに対応するコレクションというキースペース(名前空間)が存在します。]

//footnote[memo-cbs-sql][Couchbase Serverでは、@<tt>{SELECT * FROM task}のようなクエリ表現が可能です。]

//footnote[couchbase-lite-current-java-document][https://docs.couchbase.com/couchbase-lite/current/java/document.html]


=== まとめ

データモデリングの観点から各データベースについて見てきました。

データモデリングにおけるそれぞれの違いは、一次的には、それらの特徴であって、優劣ではないということができます。
一方で各OSが提供する独自の技術は、その存在意義を示すために、SQLiteと比べて優位性を強調しています。

各OSが提供する独自の技術は、そのOS専用にアプリケーションを構築する場合には、固有の能力を発揮します。
一方で、同じ目的を持ったアプリケーションをそれぞれのOSのために作成する際の合理化のために、RealmやCouchbase Liteの様な、中立のテクノロジーを活用することが考えられます。

ここでは詳細に及ぶことはできませんでしたが、ここで紹介したデータベースには、それぞれ特色があり、データベースへのクエリの方法などは違いの分かりやすい部分でしょう。

「なぜ、Couchbase Liteなのか?」という問いへの答えについて、他のデータベースとの比較という形は取りませんが、後続の章についても参考としていただければ幸いです。

====[column]データ同期機能

リモートデータベースとデータを同期することによって、異なる端末でも同じデータが扱えるようにすることは、特殊な要件ではありません。
様々なデータベースがそれぞれのやり方で、このような要件に対する配慮を示しています。

ここでは、これまで紹介したそれぞれのデータベースについて、リモートデータベースとのデータ同期機能について簡単に触れます。

MongoDB Atlasは、Realmはとのデータ同期機能@<fn>{realm-sync-overview}を提供しています。

Core Dataは、CloudKit@<fn>{setting_up_core_data_with_cloudkit}による、iCloudとの同期をサポートしています。

Roomでは、ネットワークとデータベースからページングする(Page from network and database@<fn>{room-v3-network-db})ための@<tt>{RemoteMediator}というコンポーネントが提供されています。

以上は、あくまでそれぞれのデータベースにおいて、リモートデータベースとのデータ同期という要件が考慮されていることを示すための概観に過ぎません。
それぞれについて注目すべき特徴や、あるいは、ここに挙げた以上の機能の存在、あるいは今後の機能拡張があるかもしれませんが、それらについては是非それぞれのデータベースに関する情報へ直接当っていただきますようお願いします。

====[/column]


//footnote[realm-sync-overview][https://www.mongodb.com/docs/atlas/app-services/sync/learn/overview/]
//footnote[setting_up_core_data_with_cloudkit][https://developer.apple.com/documentation/coredata/mirroring_a_core_data_store_with_cloudkit/setting_up_core_data_with_cloudkit]
//footnote[room-v3-network-db][https://developer.android.com/topic/libraries/architecture/paging/v3-network-db]
//footnote[tutorial-sync-gateway][https://docs.couchbase.com/operator/current/tutorial-sync-gateway.html]
//footnote[google-firestore-landing][https://firebase.google.com/products/firestore]
//footnote[google-firestore-quotas][https://firebase.google.com/docs/firestore/quotas]

//blankline

====[column]組み込みデータベースの観点から見たMBaaS

各パブリッククラウドから、以下のようなMobile Backend as a Serviceが提供されています。

//blankline

 * Google Firesbase
 * AWS Amplify
 * Azure Mobile Apps

//blankline

それぞれ、iOSやAndroidといったモバイルプラットフォームの差を吸収する(両方に対応する)という意味において、RealmやCouchbase Liteと似た位置づけを持っているとも言えますが、MBaaSは、本質的にはバックエンドサービスをクラウドとして提供するものであり、組み込みデータベースと単純に比較を行えるものではありません。

一方、それぞれのサービスからオフライン時を想定した機能が提供されています。
ここでは、ローカルデータベースとしての機能という観点から、それぞれについて簡単に触れます。

AWS Amplifyは、ローカル永続データストア機能として、Amplify DataStore@<fn>{amplify-datastore}を提供しています。
Amplify DataStoreでは、データモデルの定義としてGraphQLスキーマを使用します。
リモートバックエンドと同期する場合には、AWS AppSyncと連携します。その際、GraphQLをデータプロトコルとして使用します。
同期対象のリモートデータベースとして、AWSが提供するフルマネージドNoSQLドキュメント指向データベースDynamoDBがあります。

Firebaseの提供するローカルデータサポートは、一時的なオフラインのためのキャッシュとしてのものであると言えます。
Firesbaseでは、リモートデータベースとしてリレーショナルデータベースを利用することもできますが、NoSQLデータベースとしてはFirestore@<fn>{google-firestore-landing}を利用できます。

Azure Mobile Appsは、オフラインデータ同期(Offline Data Sync@<fn>{azure-mobile-apps-data-sync})機能としてローカルストアを提供します。
これは、クライアントSDKの機能(@<tt>{MobileServiceSyncTable})として位置付けられ、オンライン操作時における、@<tt>{MobileServiceTable}と同等の操作性を提供しています。
同期先のリモートデータベースとして、Azure SQL DatabaseとAzure SQL Serverを使うことができます。

クラウドプロバイダーのサービスは、常に進歩しています。
ここでの記述内容については、著者の把握できていない部分や、今後変わっていく、あるいはすでに変わっている部分も存在することでしょう。
是非、それぞれのサービスの一次情報を確認いただきますようお願いします。

====[/column]

//footnote[amplify-datastore][https://docs.amplify.aws/lib/datastore/getting-started/q/platform/js/]

//footnote[amazondynamodb-ServiceQuotas][https://docs.aws.amazon.com/amazondynamodb/latest/developerguide/ServiceQuotas.html]


//footnote[azure-mobile-apps-data-sync][https://docs.microsoft.com/ja-jp/azure/developer/mobile-apps/azure-mobile-apps/howto/data-sync]


//footnote[questions-143482][https://docs.microsoft.com/en-us/answers/questions/143482/azure-mobile-app-1.html]