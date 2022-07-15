= Couchbase Liteレプリケーション

本書の前半のCouchbase Liteに関する章では、Coucbase Liteを純粋に組み込みデータベースとして利用するための情報を提供しました。
ここでは、Sync Gatewayを用いたデータ同期(Data Sync using Sync Gateway@<fn>{android-replication})を行うケースにおける、Couchbase Liteの機能について解説します。

//footnote[android-replication][https://docs.couchbase.com/couchbase-lite/current/android/replication.html]

== レプリケーター

=== 概要


Couchbase Liteにおいて、レプリケーションを司るモジュールは@<em>{レプリケーター}と呼ばれます。

レプリケーター(クライアント)はSync Gateway(サーバー)に接続し、ローカルデータベース(Couchbase Lite)とリモートデータベース(Couchbase Server)の同期を実行します。


=== 起動と停止

次のコードは、レプリケーターを開始するまでの基本的なプロセスを示しています。

//emlist[][JAVA]{
// ローカルDBと、リモートDBのエンドポイントを指定
final ReplicatorConfiguration config = new ReplicatorConfiguration(database, new URLEndpoint(new URI("wss://10.0.2.2:4984/travel-sample")));  

// レプリケーションのタイプを指定
config.setType(ReplicatorType.PUSH_AND_PULL);


// ベーシック認証情報を指定
final BasicAuthenticator auth = new BasicAuthenticator("Username", "Password".toCharArray());

config.setAuthenticator(auth);


// レプリケーター作成
final Replicator replicator = new Replicator(config); 

// レプリケーター開始
replicator.start(); 
//}

以下は、レプリケーターを停止するコードです。

//emlist[][JAVA]{
replicator.stop(); 
//}

なお、一般的なネットワークI/Oプログラミング同様、レプリケーターの実行は通常バックグラウンドで行われます。


=== 接続先構成


@<tt>{ReplicatorConfiguration}コンストラクターは、以下の引数をとります。

//blankline

 * 同期するローカルデータベース
 * Sync Gateway接続エンドポイント


//emlist[][JAVA]{
final ReplicatorConfiguration config = new ReplicatorConfiguration(database, new URLEndpoint(new URI("wss://10.0.2.2:4984/travel-sample")));  
//}

エンドポイントには、WebSocketスキーム、@<tt>{wss:}プレフィックスを使用します。

暗号化されていないクリアテキストのネットワークトラフィック(@<tt>{ws:})を使用するには@<tt>{android:usesCleartextTraffic="true"}をマニフェスト要素に含めます。詳細は、Androidのドキュメント@<fn>{security-config-CleartextTrafficPermitted}を参照ください。
これは本番環境での利用は推奨されません。


//footnote[security-config-CleartextTrafficPermitted][https://developer.android.com/training/articles/security-config#CleartextTrafficPermitted]


=== クライアント認証

Sync Gatewayでクライアント認証を行う方法として、@<em>{ベーシック認証}と@<em>{セッション認証}を用いることができます。

@<em>{ベーシック認証}では、@<tt>{BasicAuthenticator}クラスを用いてユーザー名とパスワードを指定します。
レプリケーターは最初のリクエストでクレデンシャル情報を送信してSync Gatewayからセッションクッキーを取得し、後続のすべてのリクエストに使用します。


次の例は、ユーザー名とパスワードを指定して、レプリケーションを開始する方法を示しています。

//emlist[][JAVA]{
final BasicAuthenticator auth = new BasicAuthenticator("Username", "Password".toCharArray());
config.setAuthenticator(auth);
//}


Sync Gatewayで認証するための、もうひとつの方法は、@<em>{セッション認証}です。
セッション認証では、パブリックREST APIの@<tt>{/{db\}/_session}エンドポイントを介して、ユーザーセッションを作成します。
このAPIからの応答には、セッションIDが含まれます。

セッション認証は、Sync Gatewayでカスタム認証を行う場合に用いることができます。

次の例は、あらかじめREST APIエンドポイントから取得したセッションIDを使用してレプリケーションを開始する方法を示しています。

//emlist[][JAVA]{
config.setAuthenticator(new SessionAuthenticator("904ac010862f37c8dd99015a33ab5a3565fd8447"));
//}

クライアント認証(Client Authentication@<fn>{lbl-client-auth})に関する詳細は、ドキュメントを参照ください。

=== サーバー認証

Couchbase LiteとSync Gateway間の通信においては、基本的には、信頼できるCAによって署名されたルートを持つ証明書チェーンのみが許可されており、自己署名証明書は許可されていません。

サーバー認証(Server Authentication@<fn>{lbl-svr-auth})に関する詳細は、ドキュメントを参照ください。


//footnote[lbl-client-auth][https://docs.couchbase.com/couchbase-lite/current/android/replication.html#lbl-client-auth]

//footnote[lbl-svr-auth][https://docs.couchbase.com/couchbase-lite/current/android/replication.html#lbl-svr-auth]
//footnote[certificate-pinning-android-with-couchbase-mobile][https://blog.couchbase.com/certificate-pinning-android-with-couchbase-mobile/]

====[column]エディションによる自己署名証明書の扱いの違い

エンタープライズエディションの@<tt>{ReplicatorConfiguration}クラスには、@<tt>{setAcceptOnlySelfSignedServerCertificate}メソッドが存在し、自己署名された証明書のみを受け入れるようにレプリケーターを設定することができますが、このメソッドは、コミュニティーエディションの@<tt>{ReplicatorConfiguration}クラスでは、利用できません。

====[/column]

//blankline

=== 証明書ピン留め

証明書ピン留め(Certificate Pinning)とは、サーバーの公開鍵を、アプリケーションに事前に配置する(ピン留めする)ことを指します。
これによりアプリケーションは、この公開鍵によって信頼されたサーバーとのみ通信を行います。
証明書ピン留めは、中間者(Man-in-the-middle/MITM)攻撃の防止に役立つとされています。

Couchbase Liteは、証明書のピン留めに対応しています。
詳細はドキュメント@<fn>{lbl-cert-pinning}を参照ください。また、Couchbase Blog: Certificate Pinning in Android with Couchbase Mobile@<fn>{blog-certificate-pinning-android-with-couchbase-mobile}を参照することもできます。

====[column]相互TLS

中間者(MITM)攻撃の検出に関連する技術として、相互TLS/mTLS(mutual TLS)があります。
mTLSでは、証明書を使用した通常のTLSサーバー認証に加えて、クライアントがTLSセッションをネゴシエートするときにX.509証明書を提示し、対応する秘密鍵を所有していることをサーバーに対して証明します。
証明書ピン留めでは、アプリケーション側で通信相手のサーバーの適正性の検証が行われるのに対して、mTLSではサーバー側でクライアントから提示された証明書の照合が行われます。

Couchbase LiteとSync Gateway間の通信では、mTLSはサポートされていません。
一方、Sync GatewayとCouchbase Server間の通信では、mTLSがサポートされます。

====[/column]

//footnote[lbl-cert-pinning][https://docs.couchbase.com/couchbase-lite/current/android/replication.html#lbl-cert-pinning]

//footnote[blog-certificate-pinning-android-with-couchbase-mobile][https://blog.couchbase.com/certificate-pinning-android-with-couchbase-mobile/]
//footnote[rfc8705][https://datatracker.ietf.org/doc/html/rfc8705]

=== レプリケーション構成

レプリケーターの種類には、@<tt>{PUSH}、@<tt>{PULL}、@<tt>{PUSH_AND_PULL}の三種類があります。

次の例では、レプリケータータイプを@<tt>{PUSH}専用に設定しています。

//emlist[][JAVA]{
config.setType(ReplicatorType.PUSH);
//}


また、レプリケーターの挙動には、その時点で生じている全ての差分の同期が完了した後にSync Gatewayとの接続を終了する@<em>{ワンショットレプリケーション}と、一旦すべての差分の同期が完了した後も継続して、以降のレプリケーションのためにSync Gatewayとの接続を続ける@<em>{継続的レプリケーション}とがあります。

次の例では、継続的レプリケーションに設定しています。

//emlist[][JAVA]{
config.setContinuous(true);
//}



=== チャネル指定

レプリケーターは、通常ユーザーがアクセスできるチャネルに関連づけられた全てのドキュメントをプルします。

レプリケーター構成に、チャネル名のリストを指定することもできます。この場合、レプリケーターはそれらのチャネルでタグ付けされたドキュメントのみをプルします。

//emlist[][JAVA]{
String[] channels = {"channel1", "channel2", "channel3"};
List<String> channelList = Arrays.asList(channels);
config.setChannels(channelList);
//}


=== カスタムヘッダー設定

Sync Gatewayへのリクエストに対してカスタムヘッダーを設定できます。
レプリケーターはすべてのリクエストで設定されたヘッダーを送信します。

この機能は、Couchbase LiteとSync Gatewayの間にあるプロキシサーバーによって認証・承認ステップが実行されている場合に、追加の資格情報を渡すのに利用することができます。

以下は、カスタムヘッダーを設定する例です。

//emlist[][JAVA]{
Map<String, String> headers = new HashMap<>();
headers.put("CustomHeaderName", "Value");
config.setHeaders(headers);
//}



=== チェックポイント初期化

レプリケーターは、チェックポイントを使用して、ターゲットデータベースに送信されたドキュメントを追跡します。

この機能は通常の運用において、開発者が意識する必要はありませんが、必要に応じ、レプリケーターを開始するときにチェックポイントリセット引数を指定することができます。

チェックポイントをリセットした場合、Couchbase Liteは、過去のレプリケーションでデータの一部またはすべてが既にレプリケートされている場合でも、データベース全体をレプリケートします。

以下の例では、@<tt>{start}メソッドのリセットオプションを@<tt>{true}に設定しています。
@<tt>{false}（デフォルト）は、引数として明示的に指定する必要はありません。

//emlist[][JAVA]{
if (resetCheckpointRequired) {
  replicator.start(true); 
}
//}


== レプリケーションフィルター

レプリケーションフィルターという、コールバック関数定義を使用して、プッシュレプリケーションやプルレプリケーションの結果として保存されるドキュメントをコントロールすることができます。

コールバックされる関数は純粋関数のセマンティクスに従う必要があります。そうでなければ、長時間実行される関数によってレプリケーターの速度が大幅に低下します。コールバック関数は、スレッドセーフである必要があります。

=== プッシュフィルター

プッシュフィルターを使用すると、アプリはデータベースのサブセットをサーバーにプッシュすることができます。
これはたとえば、優先度の高いドキュメントを最初にプッシュしたり、ドラフト状態のドキュメントをスキップしたりするために用いることができます。


//emlist[][JAVA]{
config.setPushFilter((document, flags) -> flags.contains(DocumentFlag.DELETED)); 
//}

=== プルフィルター

プルフィルターを使用してプルされるドキュメントを検証し、ドキュメントの同期をスキップすることができます。

プルレプリケーションフィルターと、チャネルによるフィルタリングの違いに注意が必要です。
チャネルによるドキュメントのフィルタリングはサーバー上で実行されます。
プルレプリケーションフィルターは、ダウンロードされるドキュメントに対して適用されます。

//emlist[][JAVA]{
config.setPullFilter((document, flags) -> "draft".equals(document.getString("type"))); 
//}


ドキュメントへのアクセスが失われると、プルレプリケーションフィルターもトリガーされます。
このようなイベントでドキュメントの同期をスキップすると、ドキュメントはローカルに保持され続けることになります。
その結果、Couchbase Serverにあるドキュメントとは切り離されたドキュメントのローカルコピーが残存することになります。
Couchbase Serverに保存されているドキュメントへのそれ以降の更新はプルレプリケーションで受信されず、さらにローカル編集がプッシュされ、アクセスが取り消されているためにエラーが発生する可能性があります。

== モニタリング

Couchbase Liteは、開発者がニーズに応じて利用することのできる、レプリケーションに関する、モニタリング機能を提供します。

=== アクティビティーレベル

レプリケーターのアクティビティーレベルには、以下の種類があります。

//blankline

 * @<tt>{STOPPED}: レプリケーションが終了、あるいは致命的なエラーが発生しています。
 * @<tt>{OFFLINE}: レプリケーターはリモートホストに接続しておらず、オフラインです。
 * @<tt>{CONNECTING}: レプリケーターがリモートホストに接続中です。
 * @<tt>{IDLE}: @<tt>{IDLE}は継続的レプリケーションの場合に使用されるステータスです。レプリケーションの待機状態を示します。
 * @<tt>{BUSY}: レプリケーターがアクティブにデータを転送しています。

 以下は、アクティビティーレベルの利用例です。


//emlist[][java]{
Log.i(TAG, "The Replicator is currently " + replicator.getStatus().getActivityLevel());


if (replicator.getStatus().getActivityLevel() == ReplicatorActivityLevel.BUSY) {
    Log.i(TAG, "Replication Processing");
}
//}

=== レプリケーション進行状況監視


開発者は、レプリケーターの状態の変更イベントに応じてアクションを実施するために、リスナーを登録することができます。
これによって、たとえば、ユーザーに対してレプリケーションの進行状況を通知することができます。

@<tt>{Replicator}クラスの@<tt>{addChangeListener()}メソッドを用いて、@<tt>{Replicator}に対して、チェンジリスナーを追加します。
これにより、レプリケーターの状態の変化が非同期で通知され、登録したチェンジリスナーのコールバック関数が実行されます。

@<tt>{Replicator}の@<tt>{start()}メソッドをコールする前に、リスナーを登録することによって、@<tt>{start()}メソッドコールによるレプリケーション開始以降の通知を受け取ることができます。


以下は、アクティビティーレベルと組み合わせて、状態をログ出力する例です。

//emlist[][JAVA]{
replicator.addChangeListener(new ReplicatorChangeListener() {
    @Override
    public void changed(ReplicatorChange change) {

        if (change.getReplicator().getStatus().getActivityLevel().equals(ReplicatorActivityLevel.IDLE)) {

            Log.e("Replication Comp Log", "Schedular Completed");
        }
        if (change.getReplicator().getStatus().getActivityLevel()
            .equals(ReplicatorActivityLevel.STOPPED) || change.getReplicator().getStatus().getActivityLevel()
            .equals(ReplicatorActivityLevel.OFFLINE)) {
            Log.e("Rep schedular  Log", "ReplicationTag Stopped");
        }
    }
});
//}

レプリケーションが停止すると、アクティブなチェンジリスナーはすべて停止します。

リスナーの登録を明示的に解除したい場合には、後でリスナーを削除できるように、トークンを保存します。
@<tt>{removeChangeListener(ListenerToken token)}を使用してリスナーを削除することができます。
以下は、トークンを受け取る例です。

またここでは、上記例の、@<tt>{ReplicatorChangeListener}を継承する無名クラスの利用に代えて、ラムダ式を利用しています。

この例では、コールバック関数の中で、エラーの発生を確認しエラー内容をログ出力しています。

//emlist[][JAVA]{
ListenerToken token = replicator.addChangeListener(change -> {
    final CouchbaseLiteException err = change.getStatus().getError();
    if (err != null) {
        Log.i(TAG, "Error code ::  " + err.getCode(), err);
    }
});
//}



=== ドキュメント更新監視

レプリケーターの状態の監視だけではなく、レプリケーション中のドキュメントの更新を監視し、レプリケーションの状況に応じた処理を実装することができます。

次の例では、ドキュメントの同期状況を監視するリスナーを登録しています。

//emlist[][JAVA]{
replicator.addDocumentReplicationListener(replication -> {

    // レプリケーションタイプをログ出力
    Log.i(TAG, "Replication type: " + (replication.isPush() ? "Push" : "Pull"));
    for (ReplicatedDocument doc : replication.getDocuments()) {

        // 送受信されたドキュメントのドキュメントIDをログ出力
        Log.i(TAG, "Doc ID: " + doc.getID());

         CouchbaseLiteException err = doc.getError();
        if (err != null) {
            // エラー発生
            Log.e(TAG, "Error replicating document: ", err);
            return;
        }
        // ドキュメント削除のケース
        if (doc.getFlags().contains(DocumentFlag.DELETED)) {
            Log.i(TAG, "Successfully replicated a deleted document");
        }
    }
});
//}

上記の例で使われている列挙型@<tt>{com.couchbase.lite.DocumentFlag}には、@<tt>{DELETED}の他に、@<tt>{ACCESS_REMOVED}が定義されています。
ユーザーに対してドキュメントへのアクセス権が取り消されると、この通知が送信されます。
これは、現在の自動パージ設定に関係なく送信されます。
自動パージが有効な場合、通知の後、そのドキュメントはローカルデータベースからパージされます。

=== プッシュ保留中ドキュメント監視

@<tt>{Replicator}には、プッシュ保留中のドキュメントが待機しているかどうかを確認することができるAPIが用意されています。

//blankline

 * @<tt>{getPendingDocumentIds()}メソッドは、ローカルで変更されているが、まだサーバーにプッシュされていないドキュメントIDのリストを返します。これは、プッシュ同期の進行状況を追跡したり、アプリがエンドユーザーにステータスを視覚的に示したり、いつ安全に終了できるかを判断したりするのに役立ちます。
 * @<tt>{isDocumentPending(docId)}メソッドを個々のドキュメントがプッシュを保留しているかどうかを確認するために利用することができます。

//blankline

以下に利用例を示します。

//emlist[][JAVA]{
final Set<String> pendingDocs = replicator.getPendingDocumentIds();

for (Iterator<String> itr = pendingDocs.iterator(); itr.hasNext(); ) {
    final String docId = itr.next();

    if (replicator.isDocumentPending(docId)) {
        Log.i(TAG, "Doc ID " + docId + " is pending");
    } else {
        Log.i(TAG, "Doc ID " + docId + " is not pending");
    }
}
//}


== ネットワークエラー対応


レプリケーターがネットワークエラーを検出した場合、以下のようにケースに応じて振る舞いが異なります。


=== 恒久的なネットワークエラー

たとえば、「404 not found」、または「401 unauthorized」のような恒久的なネットワークエラーの場合、レプリケーターはステータスを@<tt>{STOPPED}に設定します。
そして、レプリケーターは即座に停止します。これは、継続的レプリケーションモードであったとしても同様です。


=== 一時的なネットワークエラー

次のエラーコードは、レプリケーターによって一時的なものと見なされます。

//blankline

 * 408: Request Timeout
 * 429: Too Many Requests
 * 500: Internal Server Error
 * 502: Bad Gateway
 * 503: Service Unavailable
 * 504: Gateway Timeout
 * 1001: DNS resolution error

//blankline

このようなエラーは、回復可能なエラーと見做され、レプリケーターはステータスを@<tt>{OFFLINE}に設定した上で、接続の再試行を行います。

この再施行は、継続的レプリケーションでは無期限に試みられますが、ワンショットレプリケーションでは一定の回数だけ試みられます。

=== 再試行設定

レプリケーターは、回復可能なネットワークエラーに対応するため、ハートビート機能と再試行ロジックを備えています。
これによって、復元力のある接続が実現されています。

レプリケーターは、ハートビートを維持することにより、接続が切断される可能性とその影響を最小限に抑えます。
ハートビートパルスの間隔は設定可能です。

接続エラーを検出した場合、レプリケーターは再施行を試みます。
再施行の回数は設定可能です。

再試行のたびに、試行間の間隔が指数関数的に増加します。個々の再施行の間の間隔はレプリケーターの指数バックオフアルゴリズムによって計算され、最大待機時間設定によって上限が定められます。
最大待機時間は設定可能です。

以下に、これらの設定を変更する例を示します。

//emlist[][JAVA]{
// ハートビートパルス間隔(秒)を設定
config.setHeartbeat(150);
// 最大再試行回数を設定
config.setMaxAttempts(20);
// 再施行最大待機時間(秒)を設定
config.setMaxAttemptWaitTime(600);
//}


ハートビートパルス間隔については、ロードバランサーやプロキシを利用している際には、それらのキープアライブ間隔を考慮して設定します。

再試行を行いたくない場合は、最大再試行回数をゼロに設定します。ワンショットレプリケーションにおける最大再試行回数のデフォルトは9回です。

最大待機時間とハートビートパルス間隔のデフォルト値は300秒(5分)です。


== ドキュメント自動パージ

=== 概要

Couchbase Liteは、ユーザーがドキュメントへのアクセス権を失った場合、そのドキュメントをローカルデータベースから自動的にパージします。

ユーザーは、以下のケースでチャネルにアクセスできなくなる可能性があります。

//blankline

 * ユーザーから、チャネルへのアクセスが直接的に失われる。
 * ユーザーがチャネルにアクセスできるロールから削除される。
 * ユーザーが割り当てられているロールがチャネルにアクセスできなくなる。

//blankline

複数のチャネルに存在するドキュメントは、ユーザーがすべてのチャネルにアクセスできなくなるまで、自動パージされません。

また、ユーザーがドキュメントへのアクセスを回復した場合には、パージされたドキュメントは自動的にプルダウンされます。


=== 自動パージ無効化

自動パージ機能はデフォルトで有効ですが、設定で無効にすることが可能です。

以下は、その設定例です。

//emlist[][JAVA]{
config.setAutoPurgeEnabled(false); 
//}

