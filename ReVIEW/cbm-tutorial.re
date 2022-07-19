
= Couchbase Mobileを体験する

== はじめに

Couchbase Mobileをモバイル/エッジコンピューティングデータプラットフォームとして利用する際には、Couchbase LiteとSync Gatewayに加え、Couchbase Serverを利用します。
さらに、モバイルアプリケーションだけでなく、Webアプリケーションでも同じサービスを提供する場合があります。
この場合、Webアプリケーションサーバーは、Couchbase Serverのクライアントとして位置付けられます。

このような環境を簡単に体験することのできるチュートリアルCouchbase Mobile Workshop@<fn>{mobile-travel-tutorial-introduction}が公開されています。
このチュートリアルに従うことによって、Sync Gatewayを用いたCouchbase LiteとCouchbase Serverとの同期を比較的簡単に体験することができます。
また、チュートリアルの素材としてモバイルアプリケーションのみではなくWebアプリケーションも提供されており、Couchbase Serverクライアントとの共存のユースケースについても確認することができます。

このチュートリアルの環境は、Dockerリポジトリーに公開されているセットアップ済みのDockerイメージを利用して構築することができます。
また、アプリケーションのコードや設定ファイルはGitHubリポジトリーで公開されています。

本章では、演習形式の実行を想定して、このチュートリアルの内容を紹介します。
演習の進め方の概要としては、はじめに環境を構築して、アプリケーションを実行できるようにします。
演習には、具体的な処理のプログラミングは含まれません。
アプリケーションが実行できるようになったら、その処理を行っているソースコードの該当箇所と照らし合わせながら、対応するアプリの挙動を確認していきます。

チュートリアルのアプリケーションは、旅行会社のサービスを扱っており、ホテルの検索とブックマークやフライトの検索と予約を行う機能を提供します。

なお、このCouchbase Mobile Workshopチュートリアルは、Swift等複数のプログラミング言語に対応していますが、ここではAndroid Javaを用いて解説します。

また、チュートリアルでは、エンタープライズエディションが利用されています。

//footnote[mobile-travel-tutorial-introduction][https://docs.couchbase.com/tutorials/mobile-travel-tutorial/introduction.html]


====[column]エンタープライズエディション利用

Couchbaseのエンタープライズエディションには、ライセンスキーやアクティベーションコードは存在しません。
また試用期間も存在せず、利用規約の定める範囲で使い続けることができます。

コミュニティーエディションは、機能や利用規模に関して定められた規約(Couchbase, Inc. Community Edition License Agreement@<fn>{community-license-agreement04272021})に基づいて商用目的で利用することができます。
規約の概要について、2021年6月に投稿されたBlog: Couchbase Modifies License of Free Community Edition Package@<fn>{couchbase-modifies-license-free-community-edition-package}を参照することができます。

エンタープライズエディションは、商用目的以外ないしプリプロダクションフェーズにおいて利用することができます。
公式のFAQ(Couchbase Licensing and Support Frequently Asked Questions@<fn>{licensing-and-support-faq})から、この点について触れている該当部分を引用します。@<fn>{faq-question}

「Customers can use Enterprise Edition free-of-charge for unlimited pre-production development and testing with forum support.@<fn>{faq-full}」

以下に翻訳を示します。

「顧客はエンタープライズエディションを、プリプロダクションの開発とテストのために、制限なく、フォーラムのサポートを利用しながら、無料で使用することができます。」


コミュニティーエディションユーザーや、エンタープライズエディションをサブスクリプションを購入せずに利用しているユーザーは、フォーラム@<fn>{forums-couchbase-com}で、質問を投稿して有志からの回答を受けたり、過去の質問への回答を検索したりすることができます。

====[/column]

//footnote[faq-question][これは、「Do I need to purchase an Enterprise Edition subscription for development, QA, and pre-production usage?」という質問への回答です。]
//footnote[faq-full][回答全文では、引用文の後に以下が続きます:For customers who require technical support during production, development, QA, pre-production testing and/or as part of a 3rd-party packaged and/or partner-provided solution, a paid subscription is required. This is inclusive of nodes and/or devices in non-production where the production usage requires an Enterprise Edition paid subscription.]
//footnote[enterprise-software-support-policy][https://www.couchbase.com/support-policy/enterprise-software]
//footnote[community-license-agreement04272021][https://www.couchbase.com/community-license-agreement04272021]
//footnote[licensing-and-support-faq][https://www.couchbase.com/licensing-and-support-faq]
//footnote[form-couchbase][https://forums.couchbase.com/]
//footnote[couchbase-modifies-license-free-community-edition-package][https://blog.couchbase.com/couchbase-modifies-license-free-community-edition-package/]
//footnote[forums-couchbase-com][https://forums.couchbase.com/]

== 環境概要

=== Couchbaseプロダクト

 * Couchbase Server v7.0.0
 * Couchbase Lite v3.0.0
 * Sync Gateway v3.0.0
 * Couchbase Python SDK 3.0.x 


=== 開発環境

 * 最新のAndroid Studio
 * Androidエミュレーター(APIレベル 22以上)
 * Android SDK 29
 * Android ビルドツール29以上
 * JDK 8以上


=== 実行環境

下記の3つの環境をDockerイメージを用いて構築します。

//blankline

 * Couchbase Server
 * Sync Gateway
 * Web/REST API Server

//blankline

Couchbase Serverについては、サンプルデータやユーザーがセットアップ済みの環境(server-sandbox)を用います。
この環境にセットアップされている内容に関心がある場合は、ドキュメント@<fn>{manual-couchbase-server}でマニュアル設定手順を確認することができます。


//footnote[736e5a05dd24e40787aba79d79615b03][https://docs.couchbase.com/tutorials/mobile-travel-tutorial/introduction.html]

//footnote[manual-couchbase-server][https://docs.couchbase.com/tutorials/mobile-travel-tutorial/android/installation/manual.html#couchbase-server]


== 環境構築

=== チュートリアルリポジトリー

はじめにmobile-travel-sampleリポジトリーを取得します。
Sync Gatewayの環境構築のために、このリポジトリーに含まれているSyng Gateway用の構成ファイルを利用します。


//cmd{
$ git clone -b master --depth 1 \
  https://github.com/couchbaselabs/mobile-travel-sample.git
//}

=== ローカルDockerネットワーク

「workshop」という名前のローカルDockerネットワークを作成します。
次のコマンドを実行します。

//cmd{
$ docker network ls
$ docker network create -d bridge workshop
//}

=== Couchbase Server

Dockerイメージを取得します。

//cmd{
$ docker pull couchbase/server-sandbox:7.0.0
//}

Dockerコンテナを起動します。

//cmd{
$ docker run -d --name cb-server --network workshop -p 8091-8094:8091-8094 \
  -p 11210:11210 couchbase/server-sandbox:7.0.0
//}


次のコマンドを使用して、「cb-server」という名前のDockerコンテナが実行されていることを確認します。

//cmd{
$ docker ps
//}

Dockerプロセスが開始されていても、サーバーが起動するまでに数秒かかる場合があります。
次のコマンドにより、Couchbase Serverのログを表示できます。

//cmd{
$ docker logs cb-server
//}

以下に、実行例を示します。

//cmd{
$ docker logs cb-server
Starting Couchbase Server -- Web UI available at http://<ip>:8091
and logs available in /opt/couchbase/var/lib/couchbase/logs
Configuring Couchbase Server.  Please wait (~60 sec)...
Configuration completed!
Couchbase Admin UI: http://localhost:8091 
Login credentials: Administrator / password
//}


Couchbase Serverの起動が完了するのを待って、Couchbase ServerのWeb管理コンソールを開きます。
URLは、次の通りです。: @<href>{http://localhost:8091}

ユーザー名を「Administrator」、パスワードを「password」としてログインします。

//image[cb-server-login][Couchbase Server Web管理コンソールのログイン画面]{
//}

ログイン後、サイドメニューから[Buckets]を選択し、travel-sampleバケットにサンプルデータが登録されていることを確認します。


//image[cb-server-buckets][Couchbase Server Web管理コンソールのBuckets画面]{
//}

=== Sync Gateway


Dockerイメージを取得します。

//cmd{
$ docker pull couchbase/sync-gateway:3.0.0-enterprise
//}

このチュートリアルでは、sync-gateway-config-travelsample.jsonという名前の構成ファイルを使用してSync Gatewayを起動します。
このファイルは、mobile-travel-sampleリポジトリーに含まれています。

接続先を確認するため、任意のテキストエディターを使用してsync-gateway-config-travelsample.jsonを開きます。

Couchbase Serverに接続するには、サーバーのアドレスを指定する必要があります。
Couchbase ServerのDockerコンテナを起動したときに@<tt>{--name}オプションに「cb-server」を指定したことを思い出してください。

以下のように、接続先の記述が「cb-server」であることを確認します。

//emlist[sync-gateway-config-travelsample.json][JSON]{
    "databases": {
        "travel-sample": {
            "import_docs": true,
            "bucket": "travel-sample",
            "server": "couchbases://cb-server",
//}


確認したファイルを使用してSync Gatewayを起動します。

ファイルが置かれているフォルダーから以下のコマンドを実行します。


Windows以外のプラットフォームでの実行方法は、以下のようになります。

//cmd{
$ docker run -p 4984-4985:4984-4985 --network workshop --name sync-gateway -d \
  -v `pwd`/sync-gateway-config-travelsample.json:/etc/sync_gateway/sync_gateway.json \
  couchbase/sync-gateway:3.0.0-enterprise -adminInterface :4985 \
  /etc/sync_gateway/sync_gateway.json
//}

Windowsでの実行方法は、以下のようになります。

//cmd{
$ docker run -p 4984-4985:4984-4985 --network workshop --name sync-gateway -d \
  -v %cd%/sync-gateway-config-travelsample.json:/etc/sync_gateway/sync_gateway.json \
  couchbase/sync-gateway:3.0.0-enterprise -adminInterface :4985 \
  /etc/sync_gateway/sync_gateway.json
//}

次のコマンドを使用して、「sync-gateway」という名前のDockerコンテナが実行されていることを確認します。

//cmd{
$ docker ps
//}

次のコマンドにより、Sync Gatewayのログを表示できます。

//cmd{
$ docker logs sync-gateway
//}

ブラウザーで次のURLにアクセスします。: @<href>{http://127.0.0.1:4984}

以下のような応答が返されることを確認します。

//emlist[][JSON]{
{"couchdb":"Welcome","vendor":{"name":"Couchbase Sync Gateway","version":"3.0"},"version":"Couchbase Sync Gateway/3.0.0(541;46803d1) EE"}
//}

=== Webアプリケーション

Dockerイメージを取得します。

//cmd{
$ docker pull connectsv/try-cb-python-v2:6.5.0-server
//}

Dockerコンテナを起動します。

//cmd{
$ docker run -it -p 8080:8080 --network workshop --name try-cb-python \
  connectsv/try-cb-python-v2:6.5.0-server
//}


コンソールに次のように出力されます。


//cmd{
Connecting to: couchbase://cb-server
couchbase://cb-server <couchbase.auth.PasswordAuthenticator object at 0x7f06d1357eb8>
 * Running on http://0.0.0.0:8080/ (Press CTRL+C to quit)
//}

ブラウザーで次のURLにアクセスします。: @<href>{http://127.0.0.1:8080}


Webアプリのログイン画面が表示されることを確認します。

//image[web-app-screen][Webアプリのログイン画面]{
//}


== モバイルアプリセットアップ

=== プロジェクト

先に取得済みのmobile-travel-sampleリポジトリーに含まれるAndroidプロジェクトをAndroid Studioで開きます。
build.gradleファイルは@<tt>{mobile-travel-sample/android/TravelSample}の下にあります。

=== 接続先確認

@<tt>{com.couchbase.travelsample.util}パッケージの@<tt>{DatabaseManager.java}を開きます。

WebアプリケーションサーバーとSync Gatewayの接続先を確認します。

以下のように、WebアプリケーションサーバーとSync Gatewayの接続先が設定されていることを確認します。

//emlist[DatabaseManager.java]{
public class DatabaseManager {
  // Use 10.0.2.2 if using Emulator(s)
    public static String APPLICATION_ENDPOINT = "http://10.0.2.2:8080/api/";
    public static String SGW_ENDPOINT = "ws://10.0.2.2:4984/travel-sample";
//}

====[column]エミュレーターからローカル環境への接続

本演習では、モバイルアプリケーションの実行確認は、エミュレーターの利用を想定しています。
エミュレーターからローカルホストで稼働しているサーバーにアクセスする際には、接続先としてエミュレーターがアサインしている特別なIPアドレスを指定します。
Andoroidエミュレーターのからローカルホストへの接続に用いるIPアドレスは、「10.0.2.2」になります。

====[/column]

=== 実行

プロジェクトをビルドして、Androidエミュレーターを使用してアプリを実行します。
 
モバイルアプリのログイン画面が表示されます。


//image[mobile-app-login][モバイルアプリのログイン画面]{
//}

これで、環境の準備が完了しました。

== モバイルアプリ設計

=== ログインオプション

モバイルアプリのログイン画面には、次のログインオプションが表示されます。

//blankline

 * SIGN IN
 * PROCEED AS GUEST

//blankline

これらのオプションは、それぞれ次の動作モードに対応しています。

//blankline

 * 同期(Sync)モード: ユーザーが入力した情報をCouchbase Serverと同期します。Webアプリケーションでログインした場合も同じデータが引き継がれます。
 * ゲスト(Guest)モード: ローカルデータのみを使ってアプリを操作します。

//blankline

モバイルアプリ内部では、それぞれのモードに対応したデータベース、Travel Sample DBとGuest DBが用いられています。

//image[app-login-options-arch][モバイルアプリのデータベースインスタンス概念図][]{
//}

(図は、Couchbase Mobile Workshopより引用)


=== 同期モードのデータモデル


同期モードでは、モバイルアプリは、Sync GatewayおよびWebアプリケーションサーバーが提供するREST APIを介して、Couchbase Serverと通信します。
同期モードで利用されるドキュメントの種類は次の通りです。

//blankline

 * @<tt>{user}
 * @<tt>{airline}
 * @<tt>{airport}
 * @<tt>{hotel}
 * @<tt>{route}

//blankline

@<tt>{user}ドキュメントは、アプリケーションで作成・更新され、Sync Gatewayを介してCouchbase Serverに同期されます。
@<tt>{user}ドキュメントには、ID、名前、パスワードのようなユーザーに関する情報の他、ユーザーが予約したフライトに関する情報も含まれます。

演習では、@<tt>{user}ドキュメントを対象にしたレプリケーションの動作を確認します。

//image[app-data-model-diagram_2][userデータモデル]{
//}

(図は、Couchbase Mobile Workshopより引用)

//blankline



//blankline

@<tt>{user}以外のドキュメントは、アプリケーションから作成・更新されることはありません。

@<tt>{hotel}と@<tt>{airport}ドキュメントは、プレビルドデータベースとして、モバイルアプリにあらかじめバンドルされています。
演習では、これらのドキュメントを利用してクエリや全文検索機能を確認します。


//image[app-data-model-diagram_1][user以外のデータの関係構造図]{
//}

(図は、Couchbase Mobile Workshopより引用)



=== ゲストモードのデータモデル

ゲストモードでは、ユーザーとしてアプリにログインしていなくても、ホテルの情報を検索・閲覧し、気になったホテルをブックマークすることができます。

モバイルアプリはゲストユーザー用のデータベースを持ちます。
このデータベースには、ユーザーによってブックマークされたホテルの情報が保存されます。

ゲストモードでは、次のタイプのドキュメントが利用されます。

//blankline

 * @<tt>{hotel}
 * @<tt>{bookmarkedhotels}

//blankline

//image[app-data-model-diagram-standalone][ローカルデータ関係構造図]{
//}

(図は、Couchbase Mobile Workshopより引用)


=== ドキュメント設計

Couchbase Liteでは、異なる種類のドキュメントが同じ名前空間に保存されます。
したがって、通常はドキュメントの種類を示すプロパティーを使用して、各エンティティーを区別します。
このアプリケーションでは、このプロパティーを「type」と名付けています。

また、このアプリケーションではドキュメントIDに、ドキュメントの種類と数値文字列との組み合わせを用いています。
ドキュメントIDに用いるドキュメント種類は、ドキュメントの@<tt>{type}プロパティーの値と一致しており、数値文字列との組み合わせにより、ドキュメントを一意に識別します。


=== プロジェクト構成

モバイルアプリの設計は、MVP(Model-View-Presenter)パターンに従っています。

クラスは機能ごとにパッケージ化されており、各パッケージには次の3つの種類のファイルが含まれます。

//blankline

 * Activity: すべてのビューロジックが存在する場所です。
 * Presenter: データのフェッチと永続化のためのロジックが存在する場所です。
 * Contract: PresenterとがActivityが実装するインターフェースです。

//blankline

演習ではCouchbase Liteの機能について、プレゼンターのコードで行われている内容を確認していきます。


== データベース操作

=== 初期化

アプリケーションで、Couchbase Liteデータベースの利用を開始するには、はじめにアプリケーションコンテキストを用いてCouchbase Liteを初期化します。 

//emlist[DatabaseManager.java][JAVA]{
    public void initCouchbaseLite(Context context) {
        CouchbaseLite.init(context);
        Database.log.getConsole().setLevel(LogLevel.DEBUG);
        appContext = context;
    }
//}

=== データベース作成

このアプリケーションでは、ユーザーごとにデータベースを作成します。

@<tt>{guest}という名前のディレクトリーにゲストユーザー用の空のデータベースを作成します。

@<tt>{DatabaseManager}の@<tt>{OpenGuestDatabase}メソッドを確認します。

@<tt>{DatabaseConfiguration}に対して、「guest」という名前のディレクトリーを設定しています。
設定した@<tt>{DatabaseConfiguration}を渡して、「guest」という名前のデータベースを作成しています。


//emlist[DatabaseManager.java][JAVA]{
    public void OpenGuestDatabase() {
        DatabaseConfiguration config = new DatabaseConfiguration();

        this.enableLogging(appContext);
        config.setDirectory(String.format("%s/guest", appContext.getFilesDir()));

        try {
            database = new Database("guest", config);
        }
        catch (CouchbaseLiteException e) {
            e.printStackTrace();
        }
    }
//}


=== ドキュメントの作成と更新


ユーザーがホテルをブックマークすると、@<tt>{bookmarkedhotels}タイプのドキュメントが作成され、@<tt>{hotels}プロパティーに、そのホテルに対応する@<tt>{hotel}ドキュメントのドキュメントIDが格納されます。
以下は、@<tt>{bookmarkedhotels}ドキュメントの例です。

//emlist[][JSON]{
{
  "type": "bookmarkedhotels",
  "hotels": ["hotel1", "hotel2"]
}
//}

ドキュメントの作成と保存について、確認します。

@<tt>{com.couchbase.travelsample.hotels}パッケージの@<tt>{HotelsPresenter.java}を開きます。

@<tt>{HotelsPresenter}の@<tt>{bookmarkHotels(Map<String, Object> hotel)}メソッドを確認します。

ここでは、まず@<tt>{DatabaseManager}からデータベースのインスタンスを取得しています。


//emlist[HotelsPresenter.java][JAVA]{
Database database = DatabaseManager.getDatabase();
//}

ホテルの情報は、@<tt>{hotel}タイプのドキュメントとして保持されています。

次のコードは、@<tt>{hotel}ドキュメントを@<tt>{guest}データベースに新しいドキュメントとして保存します。


//emlist[HotelsPresenter.java][JAVA]{
MutableDocument hotelDoc = new MutableDocument((String) hotel.get("id"), hotel);
try {
  database.save(hotelDoc);
} catch (CouchbaseLiteException e) {
  e.printStackTrace();
}
//}


次に、@<tt>{bookmarkedhotels}ドキュメントを作成します。
ドキュメントには、@<tt>{type}プロパティーの値として「bookmarkedhotels」が設定されています。
また、ブックマークされたホテルのドキュメントIDを格納するのための配列型の@<tt>{hotels}プロパティーが含まれます。

ドキュメントIDには@<tt>{user::guest}が用いられています。
ゲストユーザーのための@<tt>{bookmarkedhotels}ドキュメントはアプリにおいて唯一であり、ローカルでのみ利用されます。

//emlist[HotelsPresenter.java][JAVA]{
if (document == null) {
    HashMap<String, Object> properties = new HashMap<>();
    properties.put("type", "bookmarkedhotels");
    properties.put("hotels", new ArrayList<>());
    mutableCopy = new MutableDocument("user::guest", properties);
}
else {
    mutableCopy = document.toMutable();
}
//}

ユーザーによって選択されたホテルのIDは、@<tt>{bookmarkedhotels}ドキュメントの配列型の@<tt>{hotels}プロパティーに追加されます。

//emlist[HotelsPresenter.java][JAVA]{
MutableArray hotels =  mutableCopy.getArray(hotels).toMutable();
mutableCopy.setArray(hotels,hotels.addString((String) hotel.get(id)));
//}

最後に、ドキュメントが保存されます。

//emlist[HotelsPresenter.java][JAVA]{
try {
    database.save(mutableCopy);
} catch (CouchbaseLiteException e) {
    e.printStackTrace();
}
//}

====[column]演習: ホテルのブックマーク

 1. モバイルアプリを実行します。
 2. [PROCEED AS GUEST]を選択します。
 3. [BookmarksActivity]ページが表示されることを確認します。
 4. アプリ画面右下のベットのアイコンをタップします。
 5. [location]テキストフィールドに、「London」と入力します。Londonにあるホテルのリストが表示されます。
 6. 最初のホテルのセルをタップしてブックマークします。
 7. [BookmarksActivity]画面に新しくブックマークされたホテルが表示されていることを確認します。

====[/column]

=== ドキュメントの削除

ホテルがブックマークされると、そのホテルのドキュメントがゲストユーザー用のローカルデータベースに挿入されます。
ユーザーがホテルのブックマークを解除するときは、その反対に、挿入されたホテルドキュメントをゲストユーザーのデータベースから削除します。

ドキュメントの削除について、確認します。

@<tt>{com.couchbase.travelsample.bookmarks}パッケージの@<tt>{BookmarksPresenter.java}を開きます。

@<tt>{BookmarksPresenter}の@<tt>{removeBookmark(Map<String, Object> bookmark)}メソッドを確認します。


//emlist[BookmarksPresenter.java][JAVA]{
    @Override
    public void removeBookmark(Map<String, Object> bookmark) {
        Database database = DatabaseManager.getDatabase();
        Document document = database.getDocument((String) bookmark.get("id"));
        try {
            database.delete(document);
        }
        catch (CouchbaseLiteException e) {
            e.printStackTrace();
        }

        MutableDocument guestDoc = database.getDocument("user::guest").toMutable();
        MutableArray hotelIds = guestDoc.getArray("hotels").toMutable();
        for (int i = 0; i < hotelIds.count(); i++) {
            if (hotelIds.getString(i).equals((String) bookmark.get("id"))) {
                hotelIds.remove(i);
            }
        }

        try {
            database.save(guestDoc);
        }
        catch (CouchbaseLiteException e) {
            e.printStackTrace();
        }
    }
//}


ブックマーク解除プロセスは、@<tt>{hotel}のドキュメントを削除することに加えて、@<tt>{bookmarkedhotels}ドキュメントの@<tt>{hotels}配列からホテルIDを削除します。


====[column]演習: ブックマーク解除

 1. ブックマークされたホテルのセルを左にスワイプして、ブックマークを解除します。
 2. 結果が画面に反映されることを確認します。

====[/column]

== セキュリティー

=== アプリケーションユーザー作成


アプリケーションへのユーザー登録は、Webアプリで実施します。

Webアプリ画面でユーザーを作成すると、そのユーザーの情報を持つドキュメントがCouchbase Serverに保存されます。

さらに、Webアプリは、Sync Gateway管理REST APIを用いて、同じユーザー名を持つSync Gatewayユーザーを登録します。
Syng Gatewayユーザーは、Couchbase Serverの同期対象バケットの中のドキュメントとして管理されます。
このドキュメントは、@<tt>{_sync:user:<ユーザー名>}というドキュメントIDを持ちます。


====[column]演習: 新規ユーザー作成

 1. ブラウザーでWebアプリを開きます。
 2. 新しいユーザーを作成します。ユーザー名に「demo」、パスワードに「password」を入力して[Register]をクリックします。
 3. Webアプリへログインしたことを確認します。
 4. 次に、ブラウザーでCouchbase Server Web管理コンソールを開きます。
 5. Web管理コンソールにログインします。ID/パスワードは次の通りです。: @<tt>{Administrator/password}
 6. サイドメニューの[Buckets]を選択します。「travel-sample」バケットの[Documents]リンクをクリックします。
 7. [Document ID]というラベルの付いた入力欄に、「@<tt>{user::demo}」と入力し、[Retrieve Docs]ボタンを押下します。
 8. 検索されたドキュメントの表示を確認します。「username」プロパティーの値が「demo」であることを確認します。これはアプリケーションのユーザー情報です。
 9. 次に、ドキュメントIDが「@<tt>{_sync:user:demo}」のドキュメントを探します。これは、Sync Gatewayのユーザー情報です。

====[/column]


=== Sync Gateway パブリックREST API利用

作成したユーザーを利用して、Sync GatewayのパブリックREST APIを用いて、同期対象バケット(travel-sample)へのアクセスを確認します。

まず、ユーザー情報を指定せずに、次のコマンドを実行してみます。


//cmd{
$ curl -X GET http://localhost:4984/travel-sample/
//}

次のようなエラーが返されることを確認します。

//cmd{
{"error":"Unauthorized","reason":"Login required"}
//}


今度は、Sync Gatewayユーザーの情報を指定して、アクセスします。

@<tt>{authorization}ヘッダの値は、先に作成したユーザーの情報である「demo:password」をbase64でエンコードした文字列を使用します。

たとえば、以下のように「<ユーザー>:<パスワード>」のbase64エンコード文字列を取得することができます。

//cmd{
$ echo -n "demo:password" | base64
ZGVtbzpwYXNzd29yZA==
//}

以下のように、ヘッダーを指定してtravel-sampleバケットへのアクセスを確認します

//cmd{
$ curl -X GET http://localhost:4984/travel-sample/ \
  -H 'authorization: Basic ZGVtbzpwYXNzd29yZA=='
//}

「travel-sample」データベースの詳細が表示され、「state」が「online」であることを確認します。
以下、実施例です。


//cmd{
{"db_name":"travel-sample","update_seq":4,"committed_update_seq":4,"instance_start_time":1652662401534796,"compact_running":false,"purge_seq":0,"disk_format_version":0,"state":"Online","server_uuid":"7f951c93f14e327ffd82909948e43b57"}
//}


== クエリ

=== 単純なクエリ

モバイルアプリには、データベースに対するクエリを実行している箇所が複数あります。まず単純なクエリについて見ていきます。


@<tt>{com.couchbase.travelsample.searchflight}パッケージの@<tt>{SearchFlightPresenter.java}を開きます。

@<tt>{SearchFlightPresenter}の@<tt>{startsWith(String prefix, String tag)}メソッドを確認します。


以下では、@<tt>{type}プロパティーの値が「airport」であり、@<tt>{airportname}プロパティーが指定された検索語(@<tt>{prefix})から始まるドキュメントを検索し、そのドキュメントの@<tt>{airportname}プロパティーを取得するための@<tt>{Query}を構築しています。


//emlist[SearchFlightPresenter.java][java]{
Query searchQuery = QueryBuilder
    .select(SelectResult.expression(Expression.property("airportname")))
    .from(DataSource.database(database))
    .where(
        Expression.property("type").equalTo(Expression.string("airport"))
        .and(Expression.property("airportname").like(Expression.string(prefix + "%")))
    );
//}


次に、@<tt>{Query}の@<tt>{execute()}メソッドを使用してクエリが実行されます。
結果の各行には、@<tt>{airportname}プロパティーの値が含まれます。
検索結果は、画面表示のために@<tt>{mSearchView}の@<tt>{showAirports}メソッドに渡されます。


//emlist[SearchFlightPresenter.java][java]{
ResultSet rows = null;
try {
    rows = searchQuery.execute();
} catch (CouchbaseLiteException e) {
    Log.e("app", "Failed to run query", e);
    mSearchView.displayError(e.toString());
    return;
}

List<String> data = new ArrayList<>();
for (Result row: rows) data.add(row.getString("airportname"));
mSearchView.showAirports(data, tag);
//}

====[column]演習: 前方一致検索

 1. モバイルアプリに「demo」ユーザーとして、パスワードに「password」を使い、ログインします。
 2. 画面右下の飛行機のアイコンをタップします。
 3. 遷移先画面で、画面左上の入力欄に「Detroit」と入力します。
 4. 「Detroit」から始まる項目を持つドロップダウンリストが表示されることを確認します。

====[/column]


=== 結合(JOIN)クエリ

次に、結合(JOIN)クエリについて見ていきます。

@<tt>{bookmarkedhotels}タイプのドキュメントには、ブックマークされたホテルのドキュメントIDの配列である@<tt>{hotels}プロパティーが含まれています。


@<tt>{bookmarkedhotels}ドキュメントの@<tt>{hotels}プロパティー配列に含まれているドキュメントをフェッチするクエリを確認します。

@<tt>{BookmarksPresenter.java}から、@<tt>{fetchBookmarks()}メソッドを確認します。


まず、結合クエリの両側に対応するふたつのデータソースをインスタンス化しています。


//emlist[BookmarksPresenter.java][java]{
DataSource bookmarkDS = DataSource.database(database).as("bookmarkDS");
DataSource hotelsDS = DataSource.database(database).as("hotelDS");
//}


次に、クエリ式記述を確認します。
@<tt>{bookmark}データソースのプロパティー@<tt>{hotels}と、@<tt>{hotel}データソースのドキュメントIDに対応する@<tt>{Expression}をインスタンス化しています。


//emlist[BookmarksPresenter.java][java]{
Expression hotelsExpr = Expression.property("hotels").from("bookmarkDS");
Expression hotelIdExpr = Meta.id.from("hotelDS");
//}


さらに、@<tt>{ArrayFunction}関数式を使用して、@<tt>{Meta.id}が@<tt>{hotels}配列内に含まれるドキュメントを検索する式を定義しています。

そして、結合式を定義しています。


//emlist[BookmarksPresenter.java][java]{
Expression joinExpr = ArrayFunction.contains(hotelsExpr, hotelIdExpr);
Join join = Join.join(hotelsDS).on(joinExpr);
//}


定義した結合式を使用して、ブックマークドキュメントの@<tt>{hotels}配列で参照されているすべてのホテルドキュメントを検索するクエリを定義しています。


//emlist[BookmarksPresenter.java][java]{
Expression typeExpr = Expression.property("type").from("bookmarkDS");

SelectResult bookmarkAllColumns = SelectResult.all().from("bookmarkDS");
SelectResult hotelsAllColumns = SelectResult.all().from("hotelDS");

Query query = QueryBuilder
    .select(bookmarkAllColumns, hotelsAllColumns)
    .from(bookmarkDS)
    .join(join)
    .where(typeExpr.equalTo(Expression.string("bookmarkedhotels")));
//}


最後に、ビューを更新するためのリスナーを登録して、クエリを実行します。


//emlist[][java]{
query.addChangeListener(new QueryChangeListener() {
    @Override
    public void changed(QueryChange change) {
        ResultSet rows = change.getRows();

        List<Map<String, Object>> data = new ArrayList<>();
        Result row = null;
        while((row = rows.next()) != null) {
            Map<String, Object> properties = new HashMap<>();
            properties.put("name", row.getDictionary("hotelDS").getString("name"));
            properties.put("address", row.getDictionary("hotelDS").getString("address"));
            properties.put("id", row.getDictionary("hotelDS").getString("id"));
            data.add(properties);
        }
        mBookmarksView.showBookmarks(data);
    }
});

try {
    query.execute();
} catch (CouchbaseLiteException e) {
    e.printStackTrace();
}
//}


== 全文検索

=== FTSインデックス作成

全文検索(Full Text Search)を実行するには、FTSインデックスが作成されている必要があります。

@<tt>{DatabaseManager}の@<tt>{createFTSQueryIndex()}メソッドを確認します。


以下のコードは、@<tt>{description}プロパティーに対するFTSインデックスを作成します。


//emlist[DatabaseManager.java][JAVA]{
private void createFTSQueryIndex() {
    try {
        database.createIndex("descFTSIndex", IndexBuilder.fullTextIndex(FullTextIndexItem.property("description")));
    }
    catch (CouchbaseLiteException e) {
        e.printStackTrace();
    }
}
//}

=== FTSクエリ実行

次に、FTSクエリを確認します。

@<tt>{HotelsPresenter}の@<tt>{queryHotels(String location, String description)}メソッドを確認します。


@<tt>{match()}オペレーターを使って、@<tt>{FullTextExpression}を作成しています。

この@<tt>{match}式は、複数の項目に対して@<tt>{like}比較を行う他の@<tt>{Expression}と、論理的に@<tt>{AND}で結合されます。

結合された式は、クエリのWHERE句で使用されます。

//emlist[HotelsPresenter.java][JAVA]{
pression descExp = FullTextFunction.match("descFTSIndex", description) ;
Expression locationExp = Expression.property("country")
    .like(Expression.string("%" + location + "%"))
    .or(Expression.property("city").like(Expression.string("%" + location + "%")))
    .or(Expression.property("state").like(Expression.string("%" + location + "%")))
    .or(Expression.property("address").like(Expression.string("%" + location + "%")));

Expression searchExp = descExp.and(locationExp);

Query hotelSearchQuery = QueryBuilder
    .select(SelectResult.all())
    .from(DataSource.database(database))
    .where(
        Expression.property("type").equalTo(Expression.string("hotel"))
        .and(searchExp)
    );
//}


上記のクエリを実行した結果（@<tt>{ResultSet}）を、リスト(@<tt>{List<Map<String, Object>>})に変換して、ビュー(@<tt>{mHotelView})に渡しています。


//emlist[HotelsPresenter.java][JAVA]{
ResultSet rows = null;
try {
    rows = hotelSearchQuery.execute();
} catch (CouchbaseLiteException e) {
    e.printStackTrace();
    return;
}

List<Map<String, Object>> data = new ArrayList<Map<String, Object>>();
Result row = null;
while((row = rows.next()) != null) {
    Map<String, Object> properties = new HashMap<String, Object>();
    properties.put("name", row.getDictionary("travel-sample").getString("name"));
    properties.put("address", row.getDictionary("travel-sample").getString("address"));
    data.add(properties);
}
mHotelView.showHotels(data);
//}


====[column]演習: 曖昧検索

 1. モバイルアプリにゲストユーザーとしてログインします。
 2. モバイルアプリ画面右下のベットのアイコンをタップします。
 3. 「Location」テキストフィールドに「London」と入力します。
 4. 「Description」テキストフィールドに「pets」と入力します。
 5. 「Novotel London West」がリストされていることを確認します。

====[/column]

//footnote[c93c029a8051c98a66f8e918dd3648a7][https://docs.couchbase.com/tutorials/mobile-travel-sample/android/develop/full-text-search.html]


== レプリケーション


=== データルーティング定義

Sync Gatewayはチャネルを使用して、ユーザー間でドキュメントを共有します。
チャネル設定を行っているSync関数の内容を確認します。


@<tt>{sync-gateway-config-travelsample.json}ファイルを開きます。


@<tt>{"sync"}属性にJavaScriptによる関数定義が含まれています。
以下は、チャネル設定箇所の抜粋です。

//emlist[sync-gateway-config-travelsample.json][JAVASCRIPT]{
/* Routing */
// Add doc to the user's channel.
channel("channel." + username);
//}

=== 共有バケットアクセス


共有バケットアクセスを有効化することによって、モバイルアプリとWebアプリがCouchbase Serverの同じバケットに対して読み取りと書き込みを行うことができるようになります。

この機能は、@<tt>{enable_shared_bucket_access}を「true」に設定することで有効にされます。

関連するオプションとして、@<tt>{import_docs}があります。


sync-gateway-config-travelsample.jsonファイルの該当箇所を確認します。


//emlist[sync-gateway-config-travelsample.json][JAVASCRIPT]{
"databases": {
    "travel-sample": {
        "import_docs": "true",
        "bucket": "travel-sample",
        "server": "couchbases://cb-server",
        "enable_shared_bucket_access": true
//}


インポートフィルター機能を定義することにより、Sync Gatewayでインポートおよび処理するCouchbase Serverドキュメントを指定できます。
このアプリでは、@<tt>{user}ドキュメントのみを同期します。他のすべてのドキュメントタイプは無視します。


//emlist[sync-gateway-config-travelsample.json][JAVASCRIPT]{
function(doc) {
    /* Just ignore all the static travel-sample files */
    if (doc._deleted == true ) {
        return true;
    }
    if (doc.type == "landmark" || doc.type == "hotel" || doc.type == "airport" || doc.type =="airline" || doc.type == "route") {
        return false;
    }
    return true;
}
//}

=== レプリケーション構成

@<tt>{DatabaseManager}ファイルの@<tt>{startPushAndPullReplicationForCurrentUser(String username, String password)}メソッドを確認します。


ここでは、まず同期するSync Gatewayエンドポイントを指す@<tt>{URI}オブジェクトを初期化しています。


//emlist[DatabaseManager.java][JAVA]{
URI url = null;
try {
    url = new URI(SGW_ENDPOINT);
} catch (URISyntaxException e) {
    e.printStackTrace();
}
//}


次に、レプリケーション構成箇所を確認します。

@<tt>{ReplicatorConfiguration}を、ローカルデータベースとSync Gatewayエンドポイントを指定して構築しています。

さらに、レプリケーションのタイプを指定しています。
@<tt>{pushAndPull}が用いられ、プッシュレプリケーションとプルレプリケーションの両方が有効になっていることを示しています。

また、@<tt>{continuous}モードが@<tt>{true}に設定されています。


//emlist[DatabaseManager.java][JAVA]{
ReplicatorConfiguration config = new ReplicatorConfiguration(database, new URLEndpoint(url));
config.setType(ReplicatorType.PUSH_AND_PULL);
config.setContinuous(true);
//}


以下のように、認証資格情報が設定されています。

//emlist[DatabaseManager.java][JAVA]{
config.setAuthenticator(new BasicAuthenticator(username, password.toCharArray()));
//}


ここまで行った構成で、レプリケータを初期化しています。

//emlist[DatabaseManager.java][JAVA]{
Replicator replicator = new Replicator(config);
//}


レプリケーションの変更をリッスンするために、チェンジリスナーのコールバックブロックが登録されます。プッシュまたはプルの変更があるたびに、コールバックが呼び出されます。


//emlist[DatabaseManager.java][JAVA]{
replicator.addChangeListener(new ReplicatorChangeListener() {
    @Override
    public void changed(ReplicatorChange change) {

        if (change.getReplicator().getStatus().getActivityLevel().equals(ReplicatorActivityLevel.IDLE)) {

            Log.e("Replication Comp Log", "Schedular Completed");
        }
        if (change.getReplicator().getStatus().getActivityLevel()
            .equals(ReplicatorActivityLevel.STOPPED) || change.getReplicator().getStatus().getActivityLevel()
            .equals(ReplicatorActivityLevel.OFFLINE)) {
            // stopReplication();
            Log.e("Rep schedular  Log", "ReplicationTag Stopped");
        }
    }
});
//}


最後に、レプリケーションを開始しています。


//emlist[DatabaseManager.java][JAVA]{
replicator.start();
//}

====[column]演習: プッシュレプリケーション

 1. モバイルアプリに「demo」ユーザーとしてパスワードに「password」を使い、ログインします。このユーザーは、Webアプリを介して作成されている必要があります。
 2. モバイルアプリ画面右下の飛行機のアイコンをタップして、遷移先画面でフライトを予約します。すべての入力は初期値のままにしておきます。
 3. [lookup]リンクをタップします。
 4. フライトのリストから、最初のフライトリストを選択します。これにより、予約が行われます。
 5. Webアプリにアクセスします。
 6. 「demo」ユーザーとして、パスワードに「password」を使い、Webアプリにログインします。
 7. [Booked]タブをクリックし、予約済みのフライトのリスト画面に遷移します。
 8. モバイルアプリで予約したフライトがWebアプリのフライトリストに表示されていることを確認します。

====[/column]

//blankline


====[column]演習: プルレプリケーション

 1. Webアプリにアクセスします。
 2. 「demo」ユーザーとしてパスワードに「password」を使い、Webアプリにログインします。
 3. [Flights]タブをクリックしてフライトを予約します。
 4. [From]に「Seattle」と入力し、ドロップダウンメニューから空港(Seattle Tacoma Intl)を選択します。
 5. [To]に「San Francisco」と入力し、ドロップダウンメニューから空港(San Francisco Intl)を選択します。
 6. [Leave]と[Return]を入力します(任意の日付を用いることが可能です)。
 7. [Search]ボタンをクリックします。
 8. [Outbound Flights]リストから、最初のフライトの「Add to Basket」ボタンをクリックします。
 9. [Basketタブをクリックしてフライトのリストを表示し、「Buy」ボタンをクリックして予約を確定します。
 10. [Booked]タブをクリックします。確定済みのフライト予約が表示されます。
 11. モバイルアプリに、「demo」ユーザーとして、パスワードに「password」を使い、ログインします。
 12. モバイルアプリのフライトリストに、Webアプリで予約したフライトが表示されていることを確認します。

====[/column]

//footnote[8492c4c7efa8c654ce17fa001a2604cb][https://docs.couchbase.com/tutorials/mobile-travel-tutorial/android/develop/sync.html]

== 環境利用終了

=== コンテナ停止

Couchbase ServerとSync Gatewayを停止するには、以下のコマンドを用います。

//cmd{
$ docker stop cb-server sync-gateway
//}

Webアプリケーションを停止するには、実行中のログが出力されているターミナルでCtrl+Cを入力します。

=== コンテナ削除

以下のコマンドは、今回利用した全てのコンテナを削除します。

//cmd{
$ docker rm cb-server sync-gateway try-cb-python
//}

=== コンテナイメージ削除

以下のコマンドは、今回利用した全てのコンテナイメージを削除します。

//cmd{
$ docker rmi couchbase/server-sandbox:7.0.0 \
  couchbase/sync-gateway:3.0.0-enterprise \
  connectsv/try-cb-python-v2:6.5.0-server
//}