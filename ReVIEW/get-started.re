
= Couchbase Liteを使ってみる

== はじめに

Couchbase Liteを各プログラミング言語で利用する際の基本的な手順について解説します。

本章の記述は、アプリケーションでCouchbase Liteを利用する際に、導入として必要な最低限の情報を提供することを目的としています。
本章で紹介しているプログラミング実装は、Couchbase Liteのセットアップと基本的な機能の確認を意図したものであり、本番の開発を想定したものではないことをご認識おきください。

== Android Java

=== 確認環境

 * Android Studio Bumblebee | 2021.1.1 Patch 2 for Mac

=== プロジェクト作成


 1. Android Studioで新しい「Empty Activity」プロジェクトを作成します。
 2. 「Language」として「Java」を選択します。
 3. 「Minimum SDK」として「API 22: Android 5.1 (Lollipop)」を選択します。


=== ビルト依存関係追加

 1. Gradleファイルの@<tt>{repositories}セクションに@<code>{mavenCentral()}が存在することを確認します。存在しない場合は、追加します。
 2. Gradleファイルの@<tt>{dependencies}セクションに@<code>{implementation 'com.couchbase.lite:couchbase-lite-android:3.0.0'}を含めます。

//blankline

@<tt>{repositories}セクションや@<tt>{dependencies}セクションが定義されているファイルはGradleのバージョンによって異なります。@<tt>{settings.gradle}ファイル、または@<tt>{build.gradle}ファイルを確認してください。

=== コーディング

MainActivity.javaファイルに変更を加えます。

下記のインポート文を追加します。


//emlist[MainActivity.java][java]{
import android.content.Context;
import android.util.Log;
import com.couchbase.lite.*;
//}


@<tt>{MainActivity}クラスを以下のように編集します。


//emlist[MainActivity.java][java]{
public class MainActivity extends AppCompatActivity {

    private static final String TAG = "CBL";
    private static final String DB_NAME = "cbl";
    private Context cntx = this;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Couchbase Lite 初期化
        CouchbaseLite.init(cntx);
        Log.i(TAG,"Couchbase Lite 初期化完了");

        // データベース利用開始 (存在しない場合は、新規作成)
        Log.i(TAG, "データベース利用開始");
        DatabaseConfiguration cfg = new DatabaseConfiguration();
        Database database = null;
        try {
            database = new Database(DB_NAME, cfg);
        } catch (CouchbaseLiteException e) {
            e.printStackTrace();
        }

        // ドキュメント作成
        MutableDocument mutableDoc =
                new MutableDocument().setString("type", "user").setString("last-name", "佐藤");

        // ドキュメント保存
        try {
            database.save(mutableDoc);
        } catch (CouchbaseLiteException e) {
            e.printStackTrace();
        }

        // ドキュメント取得、変更、保存
        mutableDoc =
                database.getDocument(mutableDoc.getId())
                        .toMutable()
                        .setString("first-name", "太郎");
        try {
            database.save(mutableDoc);
        } catch (CouchbaseLiteException e) {
            e.printStackTrace();
        }

        // ドキュメント取得、変更結果確認
        Document document = database.getDocument(mutableDoc.getId());
        Log.i(TAG, String.format("Document ID : %s", document.getId()));
        Log.i(TAG, String.format("名前: %s %s", document.getString("last-name"), document.getString("first-name")));

        // クエリ実行
        try {
            ResultSet rs =
                    QueryBuilder.select(SelectResult.all())
                            .from(DataSource.database(database))
                            .where(Expression.property("type").equalTo(Expression.string("user")))
                            .execute();

            for (Result result : rs) {
                Dictionary userProps = result.getDictionary(0);
                String firstName = userProps.getString("first-name");
                String lastName = userProps.getString("last-name");
                Log.i(TAG, String.format("名前: %s %s", firstName, lastName));
            }
        } catch (CouchbaseLiteException e) {
            e.printStackTrace();
        }

        // データベースクローズ
        try {
            database.close();
        } catch (CouchbaseLiteException e) {
            e.printStackTrace();
        }
    }
}
//}


=== 実行結果確認

プロジェクトをビルドし、アプリを実行します。プログラムで行っているログ出力の内容を確認します。



プログラムの実行によって作成されたデータベースを確認するには、Device File Exploreを使うことができます。

 1. エミュレーターを起動している状態で、メニューから次のように開きます。: [View] > [Tool Window] >  [Device File Explore]
 2. Device File Exploreのツリービューで次のように展開します。: [data] > [data]
 3. アプリケーションのパッケージ名のフォルダーの中の[files]の下に作成されたデータベースを確認することができます。


=== 開発参考情報

Couchbase Liteのセットアップ方法を含め、公式ドキュメントやAPIリファレンスなど、Androidプラットフォームでの開発に関するランディングページ(Couchbase Lite on Android@<fn>{android_quickstart})からアクセスすることができます。

また、Couchbase Liteを用いたAndroid Javaアプリケーション開発について、以下のチュートリアルが公開されています。

//blankline

 * User Profile Sample: Couchbase Lite Fundamentals@<fn>{userprofile_basic}
 * User Profile Sample: Couchbase Lite Query Introduction@<fn>{userprofile_query}
 * User Profile Sample: Data Sync Fundamentals@<fn>{userprofile_sync}

//footnote[android_quickstart][https://docs.couchbase.com/couchbase-lite/current/android/quickstart.html]
//footnote[userprofile_basic][https://docs.couchbase.com/tutorials/userprofile-standalone-android/userprofile_basic.html]
//footnote[userprofile_query][https://docs.couchbase.com/tutorials/userprofile-query-android/userprofile_query.html]
//footnote[userprofile_sync][https://docs.couchbase.com/tutorials/userprofile-sync-android/userprofile_sync.html]



== Kotlin


=== 確認環境

 * Android Studio Bumblebee | 2021.1.1 Patch 2 for Mac


=== プロジェクト作成


 1. Android Studioで新しい「Empty Activity」プロジェクトを作成します。
 2. 「Language」として「Kotlin」を選択します。
 3. 「Minimum SDK」として「API 22: Android 5.1 (Lollipop)」を選択します。


=== ビルト依存関係追加


 1. @<tt>{setting.gradle}ファイルの@<tt>{dependencyResolutionManagement}セクションの@<tt>{repositories}セクションに@<code>{mavenCentral()}が存在することを確認します。存在しない場合は、追加します。
 2. アプリケーションレベルの@<tt>{build.gradle}ファイルの@<tt>{dependencies}セクションに@<code>{implementation 'com.couchbase.lite:couchbase-lite-android-ktx:3.0.0'}を含めます。


=== コーディング

MainActivity.ktファイルに変更を加えます。


下記のインポート文を追加します。


//emlist[MainActivity.kt][KOTLIN]{
import android.content.Context
import android.util.Log
import com.couchbase.lite.*
//}


@<tt>{MainActivity}クラスを以下のように編集します。


//emlist[MainActivity.kt][KOTLIN]{
class MainActivity : AppCompatActivity() {

    private var TAG = "CBL"
    private var DB_NAME = "cbla"
    private var cntx: Context = this

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // Couchbase Lite 初期化
        CouchbaseLite.init(cntx)
        Log.i(TAG, "Couchbase Lite 初期化完了")

        // データベース利用開始 (存在しない場合は、新規作成)
        Log.i(TAG, "データベース利用開始")
        val cfg = DatabaseConfigurationFactory.create()
        val database = Database(DB_NAME, cfg)

        // ドキュメント作成
        var mutableDoc = MutableDocument().setString("type", "user").setString("last-name", "佐藤")

        // ドキュメント保存
        database.save(mutableDoc)

        // ドキュメント取得、変更、保存
        mutableDoc = database.getDocument(mutableDoc.id)!!.toMutable().setString("first-name", "太郎")
        database.save(mutableDoc)


        // ドキュメント取得、変更結果確認
        val document = database.getDocument(mutableDoc.id)!!
        Log.i(TAG, "ドキュメントID: ${document.id}")
        Log.i(TAG, "名前: ${document.getString("last-name")} ${document.getString("first-name")}")

        // クエリ実行
        val rs = QueryBuilder.select(SelectResult.all())
            .from(DataSource.database(database))
            .where(Expression.property("type").equalTo(Expression.string("user")))
            .execute()

        for (result in rs) {
            val userProps = result.getDictionary(0)
            val firstName = userProps!!.getString("first-name")
            val lastName = userProps.getString("last-name")
            Log.i(TAG, "名前: ${lastName} ${firstName}")
        }

        // データベースクローズ
        database.close()
    }
//}




=== 実行結果確認

Android Javaの場合と同様のため省略します。

=== 開発参考情報

Kotlinでの開発に関する情報は、先に紹介したAndroidプラットフォームでの開発に関するランディングページ(Couchbase Lite on Android@<fn>{android_quickstart})からアクセスすることができます。

Couchbase Kotlin APIは、Java APIと同等の機能を備えています。
Kotlin開発者は、Java APIとの完全な互換性に加えて、Kotlin拡張機能を利用できます。
Kotlin拡張機能には以下が含まれています。

//blankline

@<tt>{Flow}@<fn>{kotlin-flow}機能を使用して、データベースやドキュメント等のCouchbase Liteオブジェクトの変更を監視することができます。

//footnote[kotlin-flow][https://developer.android.com/kotlin/flow]

//emlist[][KOTLIN]{
GlobalScope.launch(Dispatchers.Default) {
       userprofileDatabase?.databaseChangeFlow()?
          .map {
               it.documentIDs
           }
           .onEach { ids ->  for (docId in ids) {
               val doc: Document =
                   userprofileDatabase.getDocument(docId)
               if (doc != null) {
                   Log.i("DatabaseChangeEvent", "Document was added/updated")
               } else {
                   Log.i("DatabaseChangeEvent", "Document was deleted")
               }
            }}
            .catch { throw it }
            .collect()
           }
       }
   }
}
//}


名前付きパラメーターを活用してデータベースやレプリケーターなどの各種プロパティー設定を行うことができます。

//emlist[][KOTLIN]{
val config = ReplicatorConfigurationFactory.create(
   database = userProfileDatabase,
   target = URLEndpoint(url),
   type = ReplicatorType.PULL,
   continuous = true,
   authenticator = BasicAuthenticator(username, password),
   channels = Arrays.asList("channel1"))
//}

メソッドのパラメーターと返り値に対して@<tt>{@Nullable}または@<tt>{@NonNull}アノテーションを利用することができます。

//emlist[][KOTLIN]{
@NonNull
public final ReplicatorConfiguration setChannels(@Nullable List<String> channels) {
...
}
//}

Kotlin拡張APIの詳細については、API Doc@<fn>{couchbase-lite-android-ktx}を参照ください。

//footnote[couchbase-lite-android-ktx][https://docs.couchbase.com/mobile/3.0.0/couchbase-lite-android-ktx/]



== Swift

 Swiif開発のために、Couchbase Liteを利用する場合、ダウンロードサイトからダウンロードする他、Carthage、CocoaPods、そしてSwiftパッケージマネージャーを利用する方法があります。

ここでは、Swiftパッケージマネージャーを利用したセットアップについて説明します。

その他の方法については、ドキュメント@<fn>{swift-gs-install}を参照ください。


//footnote[Carthage][https://github.com/Carthage/Carthage]

//footnote[swift-gs-install][https://docs.couchbase.com/couchbase-lite/current/swift/gs-install.html]

=== 確認環境
 * Xcode Version 12.4

=== プロジェクト作成

 1. Xcodeで、新しいプロジェクトを作成します。
 2. iOSプラットフォームの[App]アプリケーションを選択します。
 3. [Interface]として、「SwiftUI」と「UIKit」のいずれかを選択します。

=== Swiftパッケージマネージャーによるセットアップ

Swiftパッケージマネージャーを使用して、Couchbase Liteをセットアップする手順を紹介します(Swiftパッケージマネージャーを使用してCouchbase Lite SwiftをインストールするにはXcodeのバージョン12以上が必要です)。

//blankline

サイドメニューのツリービューの[PROJECT]セクションのプロジェクト名(~.xcodepojファイル)を選択し、[Swift Packages]タブを開きます。


//image[swift_3][Swift Packages画面]{
//}


[+]アイコンを押下し、[Choose Package Repository]ダイアログを表示します。


入力欄に次のURLを入力します。

//blankline

 @<tt>{https://github.com/couchbase/couchbase-lite-ios.git}

//blankline

エンタープライズエディションを利用する場合は、以下のURLを利用します。

//blankline

@<tt>{https://github.com/couchbase/couchbase-lite-swift-ee.git}


//image[choose-package-repository][Choose Package Repositoryダイアログ]{
//}


[Next]ボタンを押下します。
[Choose Package Options]ダイアログが表示されます。
バージョンを確認します。


//image[choose-package-options][Choose Package Optionsダイアログ]{
//}


[Next]ボタンを押下します。

パッケージのプロジェクトへの追加を確認するダイアログが表示されます。
[Choose package products and targes:]テーブルに、「CouchbaseLiteSwift」パッケージが表示されます。
右端のチェックボックスをチェックし、[Finish]ボタンを押下します。


[Swift Packages]タブの[Packages]セクションに、追加されたパッケージの名前、バージョン、URLが表示されます。


//image[package-section][Packagesセクション]{
//}


これで、Couchbase Liteをアプリで使用できるようになりました。


=== コーディング


「UIKit」を選択した場合は、ViewController.swiftを編集します。

「SwiftUI」を選択した場合は、ContentView.swiftを編集します。

下記のインポート文を追加します。


//emlist[MainActivity.kt][KOTLIN]{
import CouchbaseLiteSwift
//}

下記の関数を追加します。

//emlist[][swift]{
func getStarted () {
    // データベース作成または取得（再実行時）
    let database: Database
    do {
        database = try Database(name: "mydb")
    } catch {
        fatalError("Error opening database")
    }

    // ドキュメント作成
    let mutableDoc = MutableDocument().setString("佐藤", forKey: "lastname")
        .setString("user", forKey: "type")

    // ドキュメント保存
    do {
        try database.saveDocument(mutableDoc)
    } catch {
        fatalError("Error saving document")
    }

    // ドキュメント取得、変更、保存
    if let mutableDoc = database.document(withID: mutableDoc.id)?.toMutable() {
        mutableDoc.setString("太郎", forKey: "firstname")
        do {
            try database.saveDocument(mutableDoc)

            let document = database.document(withID: mutableDoc.id)!
            print("ドキュメントID: \(document.id)!)")
            print("名前： \(document.string(forKey: "lastname")!) \(document.string(forKey: "firstname")!)")
        } catch {
            fatalError("Error updating document")
        }
    }

    // クエリ
    print("クエリ実行")
    let query = QueryBuilder
        .select(SelectResult.all())
        .from(DataSource.database(database))
        .where(Expression.property("type").equalTo(Expression.string("user")))

    do {
        let result = try query.execute()
        print("ユーザー数: \(result.allResults().count)")
    } catch {
        fatalError("Error running the query")
    }

    do {
        try database.close()
    } catch {
        fatalError("Error running the query")
    }
}
//}


追加した関数がアプリ起動時に実行されるようにします。

//emlist[ContentView.swift][swift]{
import SwiftUI
import CouchbaseLiteSwift


struct ContentView: View {
    var body: some View {
        Text("Hello, world!")
            .padding()
            .onAppear {
                getStarted ()
            }
    }
    
    func getStarted () {
//}

//emlist[ViewController.swift][swift]{
import UIKit
import CouchbaseLiteSwift

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()

        getStarted()
    }

    func getStarted () {
//}


=== 実行結果確認


プロジェクトをビルドしてアプリを実行します。プログラムで行っているログ出力の内容を確認します。


以下のように、作成されたデータベースファイルを確認できます。

//cmd{
$ cd ~/Library/Developer/CoreSimulator/Devices
$ find . -name "mydb.cblite2"
./5C245945-C956-495A-B7CB-D646905E95C7/data/Containers/Data/Application/4E0B4959-9D0B-4181-A2E6-F3D3A233D5A9/Library/Application Support/CouchbaseLite/mydb.cblite2

//}

=== 開発参考情報



Couchbase Liteのセットアップ方法を含め、公式ドキュメントやAPIリファレンスなど、公式ドキュメントやAPIリファレンスまで、Swiftでの開発に関するランディングページ(Couchbase Lite on Swift@<fn>{swift_quickstart})からアクセスすることができます。

また、Couchbase Liteを用いたSwiftアプリケーション開発について、以下のチュートリアルが公開されています。

//blankline

 * User Profile Sample: Couchbase Lite Fundamentals@<fn>{swift_userprofile_basic}
 * User Profile Sample: Couchbase Lite Query Introduction@<fn>{swift_userprofile_query}
 * User Profile Sample: Data Sync Fundamentals@<fn>{swift_userprofile_sync}

//footnote[swift_quickstart][https://docs.couchbase.com/couchbase-lite/current/swift/quickstart.html]

//footnote[swift_userprofile_basic][https://docs.couchbase.com/tutorials/userprofile-standalone/userprofile_basic.html]
//footnote[swift_userprofile_query][https://docs.couchbase.com/tutorials/userprofile-query/userprofile_query.html]
//footnote[swift_userprofile_sync][https://docs.couchbase.com/tutorials/userprofile-sync/userprofile_sync.html]

== Objective-C

Objective-C開発のために、Couchbase Liteを利用する場合、ダウンロードサイトからダウンロードする他、Carthage、CocoaPodsを利用する方法があります。

ここでは、Carthage@<fn>{Carthage}を利用したセットアップについて説明します。

その他の方法については、ドキュメント@<fn>{objc-gs-install}を参照ください。


//footnote[Carthage][https://github.com/Carthage/Carthage]

//footnote[objc-gs-install][https://docs.couchbase.com/couchbase-lite/current/objc/gs-install.html]

=== 確認環境

 * Xcode Version 12.4
 * Carthage 0.38.0



=== プロジェクト作成

 1. Xcodeで、新しいプロジェクトを作成します。
 2. iOSプラットフォームの[App]アプリケーションを選択します。
 3. [Interface]として、「Storyboard」を選択し、[Language]に、「Objective-C」を使います。


=== Carthageによるセットアップ


Cartfileを作成し、以下の内容を記載します。


//emlist[Community Editionの場合][Ruby]{
binary "https://packages.couchbase.com/releases/couchbase-lite-ios/carthage/CouchbaseLite-Community.json" ~> 3.0.0
//}



//emlist[Enterprise Editionの場合][Ruby]{
binary "https://packages.couchbase.com/releases/couchbase-lite-ios/carthage/CouchbaseLite-Enterprise.json" ~> 3.0.0
//}


Cartfileを作成した場所で、以下のコマンドを実行します。

//cmd{
$ carthage update --platform ios
//}


@<tt>{Carthage/Build/}に作成された@<tt>{CouchbaseLite.xcframework}をXcodeのナビゲーターエリアにドラッグします。


//image[objective-c_1][Xcodeナビゲーター画面][]{
//}


サイドメニューのツリービューの[TARGETS]セクションのプロジェクト名を選択し、[General]タブの [Framework, Libraries, and Embeded Content]セクションを確認します


//image[objective-c_2][Xcode General画面]{
//}


=== コーディング


ViewController.mの@<tt>{viewDidLoad}メソッドを編集します。


下記のインポート文を追加します。


//emlist[ViewController.m][obj-c]{
#import <CouchbaseLite/CouchbaseLite.h>
//}


ViewController.mの全体を示します。


//emlist[ViewController.m][obj-c]{
#import "ViewController.h"

#import <CouchbaseLite/CouchbaseLite.h>

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    NSError *error;
    CBLDatabase *database = [[CBLDatabase alloc] initWithName:@"mydb" error:&error];

    // ドキュメント作成
    CBLMutableDocument *mutableDoc = [[CBLMutableDocument alloc] init];
    [mutableDoc setString:@"佐藤" forKey:@"lastname"];
    [mutableDoc setString:@"user" forKey:@"type"];

    // ドキュメント保存
    [database saveDocument:mutableDoc error:&error];

    // ドキュメント取得、変更、保存
    CBLMutableDocument *mutableDoc2 =
        [[database documentWithID:mutableDoc.id] toMutable];
    [mutableDoc2 setString:@"太郎" forKey:@"firstname"];
    [database saveDocument:mutableDoc2 error:&error];


    CBLDocument *document = [database documentWithID:mutableDoc2.id];
    // ドキュメントIDとプロパティーの出力
    NSLog(@"ドキュメントID: %@", document.id);
    NSLog(@"タイプ: %@", [document stringForKey:@"type"]);
    NSLog(@"ユーザー名: %@ %@", [document stringForKey:@"lastname"], [document stringForKey:@"firstname"]);

    // クエリ作成
    CBLQueryExpression *type =
        [[CBLQueryExpression property:@"type"] equalTo:[CBLQueryExpression string:@"user"]];
    CBLQuery *query = [CBLQueryBuilder select:@[[CBLQuerySelectResult all]]
                                        from:[CBLQueryDataSource database:database]
                                        where:type];

    // クエリ実行
    CBLQueryResultSet *result = [query execute:&error];
    NSLog(@"ユーザー数: %lu", (unsigned long)[[result allResults] count]);

    // データベースクローズ
    if (![database close:&error])
        NSLog(@"Error closing db:%@", error);

}


@end
//}

=== 実行結果確認

プロジェクトをビルドしてアプリを実行します。プログラムで行っているログ出力の内容を確認します。




=== 開発参考情報


Couchbase Liteのセットアップ方法を含め、公式ドキュメントやAPIリファレンスなど、Objective-Cでの開発に関するランディングページ(Couchbase Lite on Objective-C@<fn>{objc_quickstart})からアクセスすることができます。


//footnote[objc_quickstart][https://docs.couchbase.com/couchbase-lite/current/objc/quickstart.html]


== C#.NET


=== 確認環境

 * Visual Studio for Mac COMMUNITY 8.10.2 (build 4)


=== プロジェクト作成

C#.NETでCouchbase Liteを使う方法を簡潔に示すため、コンソールアプリとしてプロジェクトを作成します。
ここでは、フレームワークに「.NET5.0」を選んでいます。


=== NuGetによるセットアップ

 1. メニューから、プロジェクト > 「NuGetパッケージの管理...(Manage NuGet Packages...)」を開きます。
 2. 右上の「検索」フォームに「couchbase」と入力し、表示された項目から「Couchbase.Lite」を選びます。これは、コミュニティーエディションにあたります。エンタープライズエディションの場合は、「Couchbase.Lite.Enterprise」を選択します。
 3. 「パッケージの追加」ボタンを押下し、ダイアログの指示に従って進めていくと、Couchbase.Liteが依存関係に追加されます。

//image[csharp_nuget][NuGetパッケージ管理ダイアログ]{
//}


//image[csharp_dependency][依存関係ツリービュー]{
//}


=== コーディング

Program.csファイルを編集します。

下記のインポート文を追加します。


//emlist[Program.cs][csharp]{
using Couchbase.Lite;
using Couchbase.Lite.Query;
//}


@<tt>{Main}の内容を以下のように編集します。

//emlist[Program.cs][csharp]{
static void Main(string[] args)
{

    // データベース利用開始 (存在しない場合は、新規作成)
    Console.WriteLine("データベース利用開始");
    var database = new Database("mydb");

    // ドキュメント作成・保存
    string id = null;
    using (var mutableDoc = new MutableDocument())
    {
        mutableDoc.SetString("lastname", "佐藤")
            .SetString("type", "user");

        database.Save(mutableDoc);
        id = mutableDoc.Id;
    }

    // ドキュメント取得・更新
    using (var doc = database.GetDocument(id))
    using (var mutableDoc = doc.ToMutable())
    {
        mutableDoc.SetString("firstname", "太郎");
        database.Save(mutableDoc);

        using (var docAgain = database.GetDocument(id))
        {
            Console.WriteLine($"ドキュメントID: {docAgain.Id}");
            Console.WriteLine($"名前: {docAgain.GetString("lastname")} {docAgain.GetString("firstname")}");
        }
    }

    // クエリ実行
    using (var query = QueryBuilder.Select(SelectResult.All())
        .From(DataSource.Database(database))
        .Where(Expression.Property("type").EqualTo(Expression.String("user"))))
    {
        // Run the query
        var result = query.Execute();
        Console.WriteLine($"ユーザー数: {result.AllResults().Count}");
    }

    // データベースクローズ
    database.Close();

}
//}

=== 実行結果確認

プロジェクトをビルドしてアプリを実行します。プログラムで行っているログ出力の内容を確認します。


下記のように作成されたデータベースを確認することができます。


//cmd{
$ find bin -name "*.cblite2"
bin/Debug/net5.0/CouchbaseLite/mydb.cblite2
//}


=== 開発参考情報


Couchbase Liteのセットアップ方法を含め、公式ドキュメントやAPIリファレンスなど、C#.Netでの開発に関するランディングページ(Couchbase Lite on C#.Net@<fn>{csharp_quickstart})からアクセスすることができます。


//footnote[csharp_quickstart][https://docs.couchbase.com/couchbase-lite/current/csharp/quickstart.html]

== C/C++


=== 確認環境

 * Raspberry Pi 3 Model B+

OSの情報について、実環境での出力結果を示します。

//cmd{
$ cat /etc/debian_version
9.4
$ cat /etc/issue
Raspbian GNU/Linux 9 \n \l
$ uname -a
Linux raspberrypi 4.14.50-v7+ #1122 SMP Tue Jun 19 12:26:26 BST 2018 armv7l GNU/Linux
//}

=== APTによるセットアップ


UbuntuおよびDebianプラットフォームにCouchbase Liteをインストールする場合、Advanced Package Toolを使用することができます。


依存関係を含め、Couchbase Liteを自動的に取得してインストールするために必要となるメタパッケージをダウンロードします。

URLは、以下です。

//blankline

https://packages.couchbase.com/releases/couchbase-release/couchbase-release-1.0-noarch.deb

//blankline

たとえば、以下のようにcurlコマンドを用いることができます。

//cmd{
$ curl -O https://packages.couchbase.com/releases/couchbase-release/couchbase-release-1.0-noarch.deb
//}


メタパッケージをインストールします。

//cmd{
$ sudo apt install ./couchbase-release-1.0-noarch.deb
//}


ローカルパッケージデータベースを更新します


//cmd{
$ sudo apt update
//}


パッケージをインストールします。


以下は、コミュニティーエディションを使う場合の例です。


//cmd{
$ sudo apt install libcblite-dev-community
//}

エンタープライズエディションを利用する場合は、以下のように行います。


//cmd{
$ sudo apt install libcblite-dev
//}

開発用のパッケージをインストールする例を紹介しましたが、ランタイムのみのパッケージをインストールする場合は、パッケージ名はそれぞれ@<tt>{libcblite-community}、@<tt>{libcblite}となります。

=== コーディング


下記の内容を持つファイルを作成します。


//emlist[cbltest.c][c]{
#include "cbl/CouchbaseLite.h"

int main(void) {
    CBLError err;
    CBLDatabase* database = CBLDatabase_Open(FLSTR("mydb"), NULL, &err);

    if(!database) {
        fprintf(stderr, "Error opening database (%d / %d)\n", err.domain, err.code);
        FLSliceResult msg = CBLError_Message(&err);
        fprintf(stderr, "%.*s\n", (int)msg.size, (const char *)msg.buf);
        FLSliceResult_Release(msg);
        return 1;
    }

    // ドキュメント作成
    CBLDocument* mutableDoc = CBLDocument_Create();
    FLMutableDict properties = CBLDocument_MutableProperties(mutableDoc);
	FLMutableDict_SetString(properties, FLSTR("type"), FLSTR("user"));
    FLMutableDict_SetString(properties, FLSTR("lastname"), FLSTR("佐藤"));

    // ドキュメント保存
    CBLDatabase_SaveDocument(database, mutableDoc, &err);
    if(!CBLDatabase_SaveDocument(database, mutableDoc, &err)) {
        fprintf(stderr, "Error saving a document (%d / %d)\n", err.domain, err.code);
        FLSliceResult msg = CBLError_Message(&err);
        fprintf(stderr, "%.*s\n", (int)msg.size, (const char *)msg.buf);
        FLSliceResult_Release(msg);
        return 1;
    }

    // ドキュメントIDを保存し、ドキュメントのメモリーを解放
    // (注. FLSliceResultやFLStringResultとして確保した変数は明示的な解放が必要）
    FLStringResult id = FLSlice_Copy(CBLDocument_ID(mutableDoc));
    CBLDocument_Release(mutableDoc);

    // ドキュメント取得
    mutableDoc = 
        CBLDatabase_GetMutableDocument(database, FLSliceResult_AsSlice(id), &err);
    if(!mutableDoc) {
        fprintf(stderr, "Error getting a document (%d / %d)\n", err.domain, err.code);
        FLSliceResult msg = CBLError_Message(&err);
        fprintf(stderr, "%.*s\n", (int)msg.size, (const char *)msg.buf);
        FLSliceResult_Release(msg);
        return 1;
    }

    // ドキュメント更新・保存
    properties = CBLDocument_MutableProperties(mutableDoc);
    FLMutableDict_SetString(properties, FLSTR("firstname"), FLSTR("太郎"));
    if(!CBLDatabase_SaveDocument(database, mutableDoc, &err)) {
        fprintf(stderr, "Error saving a document (%d / %d)\n", err.domain, err.code);
        FLSliceResult msg = CBLError_Message(&err);
        fprintf(stderr, "%.*s\n", (int)msg.size, (const char *)msg.buf);
        FLSliceResult_Release(msg);
        return 1;
    }

    // リードオンリーでドキュメントを取得（注. constを指定しています)
    const CBLDocument* docAgain = 
        CBLDatabase_GetDocument(database, FLSliceResult_AsSlice(id), &err);
    if(!docAgain) {
        fprintf(stderr, "Error getting a document (%d / %d)\n", err.domain, err.code);
        FLSliceResult msg = CBLError_Message(&err);
        fprintf(stderr, "%.*s\n", (int)msg.size, (const char *)msg.buf);
        FLSliceResult_Release(msg);
        return 1;
    }

    // ここでは、コピーを行っていないため、後のメモリー解放は不要 (注.下記ではFLStringを利用し、FLStringResultでないことに留意)
    FLString retrievedID = CBLDocument_ID(docAgain);
    FLDict retrievedProperties = CBLDocument_Properties(docAgain);
    FLString retrievedType = FLValue_AsString(FLDict_Get(retrievedProperties, FLSTR("type")));
    printf("ドキュメントID: %.*s\n", (int)retrievedID.size, (const char *)retrievedID.buf);
    printf("タイプ: %.*s\n", (int)retrievedType.size, (const char *)retrievedType.buf);

    CBLDocument_Release(mutableDoc);
    CBLDocument_Release(docAgain);
    FLSliceResult_Release(id);

    // クエリ作成と実行
    int errorPos;
    CBLQuery* query = CBLDatabase_CreateQuery(database, kCBLN1QLLanguage, FLSTR("SELECT * FROM _ WHERE type = \"user\""), &errorPos, &err);
    if(!query) {
        fprintf(stderr, "Error creating a query (%d / %d)\n", err.domain, err.code);
        FLSliceResult msg = CBLError_Message(&err);
        fprintf(stderr, "%.*s\n", (int)msg.size, (const char *)msg.buf);
        FLSliceResult_Release(msg);
        return 1;
    }
    CBLResultSet* results = CBLQuery_Execute(query, &err);
    if(!results) {
        fprintf(stderr, "Error executing a query (%d / %d)\n", err.domain, err.code);
        FLSliceResult msg = CBLError_Message(&err);
        fprintf(stderr, "%.*s\n", (int)msg.size, (const char *)msg.buf);
        FLSliceResult_Release(msg);
        return 1;
    }
    while(CBLResultSet_Next(results)) {
        FLDict dict = FLValue_AsDict(CBLResultSet_ValueForKey(results, FLSTR("_")));

        FLString firstname = FLValue_AsString(FLDict_Get(dict, FLSTR("firstname")));
		FLString lastname = FLValue_AsString(FLDict_Get(dict, FLSTR("lastname")));

        printf("名前: %.*s", (int)lastname.size, (const char *)lastname.buf);
		printf(" %.*s\n", (int)firstname.size, (const char *)firstname.buf);
    }

    CBLResultSet_Release(results);
    CBLQuery_Release(query);

    // データベースクローズ
    CBLDatabase_Close(database, &err);

    return 0;
}
//}

=== 実行結果確認

下記のようにコンパイルします。


//cmd{
$ gcc -g -o cbltest cbltest.c -lcblite
//}


コンパイルの結果、生成された@<tt>{cbltest}を実行します。

//cmd{
$ ./cbltest
//}

プログラムで行っているログ出力の内容を確認します。


実行後、下記のようにCouchbase Liteデータベースファイルが作成されます。


//cmd{
$ ls -1
mydb.cblite2
//}

=== 開発参考情報


Couchbase Liteのセットアップ方法を含め、公式ドキュメントやAPIリファレンスなど、C/C++での開発に関するランディングページ(Couchbase Lite on C@<fn>{c_quickstart})からアクセスすることができます。


//footnote[c_quickstart][https://docs.couchbase.com/couchbase-lite/current/c/quickstart.html]



