
= Couchbase Mobile設計パターン


Couchbase Mobileを使ったアプリケーションの設計パターンをカテゴリー毎に紹介します。

本章の記述は、Couchbase Blog: Best Practices For Using Couchbase Mobile : Part 1@<fn>{best-practices-common-patterns-couchbase-mobile-part1}とBest Practices and Patterns with Couchbase Mobile - Part 2@<fn>{best-practices-couchbase-mobile-database-sync-part2}を参考にしています。


//footnote[best-practices-common-patterns-couchbase-mobile-part1][https://blog.couchbase.com/best-practices-common-patterns-couchbase-mobile-part1/]
//footnote[best-practices-couchbase-mobile-database-sync-part2][https://blog.couchbase.com/best-practices-couchbase-mobile-database-sync-part2/]



== ユーザーエクスペリエンス向上

=== データ更新の優先順位制御

アプリを利用開始時、最低限必要なデータセットを優先的にサーバーから同期することができれば、ユーザーは残りのデータが同期されるのを待たずに操作に取り掛かることができます。

このように、アプリがサーバーから受信する情報を優先順位によって制御することによって、ユーザーエクスペリエンスを向上させることができます。

//blankline

Sync Gatewayでは、チャネルを使用してドキュメントを分類しますが、この際、優先度に基づいてドキュメントを分類し、チャネル毎に同期の方法を変えることによって、ドキュメントを同期する優先順位を制御することができます。

たとえば、起動時にチャネルフィルターを指定して優先度の高いチャネルのワンショットプルレプリケーションを実行します。
そして、優先順位の高いドキュメントのレプリケーションが完了した後に、残りのチャネルのためのレプリケーションを開始します。

また、優先度の高いチャネルを継続的レプリケーションにより最新の状態に保ちながら、それ以外のチャネルについては、オンデマンドでワンショットレプリケーションを実行することも考えられます。

=== バックグラウンドでのデータ更新

ユーザーが、アプリをアクティブに（フォアグラウンドで）使用していない間に、バックグラウンドでアプリのデータを最新の状態に保つことで、ユーザーがアプリ利用を開始した際に、データ更新を待つ時間を短縮することができます。

このように、レプリケーションの実行タイミングを制御することによって、ユーザーエクスペリエンスを向上させることができます。

//blankline

アプリがバックグラウンドにプッシュされると、Couchbase Liteのレプリケーションはオフラインモードに移行します。

モバイルアプリのバックグラウンドサポートは、プラットフォームによって異なります。

たとえば、Androidでは、Work Manager@<fn>{workmanager}を使用して、ワンショットレプリケーションをスケジュールし、バックグラウンドで非同期に実行することが考えられます。

iOSでは、「Appのバックグラウンド更新」を活用することが考えられます。

また、サーバー側のデータ更新をトリガーにしてレプリケーションを実行するために、AndroidではFirebase Cloud Messaging(FCM)@<fn>{cloud-messaging}を、iOSではApple Push Notification Sevice(APNS)@<fn>{usernotifications}を使用することも考えられます。

//footnote[workmanager][https://developer.android.com/topic/libraries/architecture/workmanager]
//footnote[cloud-messaging][https://firebase.google.com/docs/cloud-messaging]
//footnote[usernotifications][https://developer.apple.com/documentation/usernotifications]



== 同期データの最適化

=== ローカルデータと同期データの分離

モバイルアプリで利用するデータの必ずしも全てをリモートデータベースと同期する必要があるわけではなく、ローカルでのみ利用するデータが存在する場合があります。


//blankline

ひとつのアプリ内で、複数のCouchbase Liteデータベースインスタンスを使うことができます。

リモートと同期する必要のないデータのためにローカルデータ専用のインスタンスを利用することが考えられます。
このローカル専用データベースには、データ同期のためのレプリケーターを構成しません。


また、リモートと同期を行う場合であっても、ローカルでのみ作成および更新するデータセットと、リモートでのみ作成・更新するデータセットに対して、別のデータベースインスタンスを利用し、それぞれプッシュ専用レプリケーター、プル専用レプリケーターを用いることも考えられます。

ただし、Couchbase Liteではデータベースインスタンスを跨がる結合クエリを実行することができないため、同期対象データとローカル専用データ間で結合クエリを実行する要件がある場合には、別の方法を検討する必要があります。


=== データ同期対象のフィルタリング

データ同期の必要性有無に応じて別々のデータベースインスタンスを利用することは簡単な方法ですが、データを同期するタイミングを細かく制御したい場合があります。
また、ローカル専用データと同期対象データとの間で結合クエリを利用する必要がある場合には、ひとつのデータベースインスタンス内で、データ同期の対象を制御します。

//blankline

レプリケーターにフィルターを設定して、サーバーにプッシュされるドキュメントを決定することができます。この機能を利用して、いつどのドキュメントをサーバーに同期するかを制御できます。

フィルターでは、ドキュメントのプロパティーを検査して、ドキュメントを同期するかどうかを判断します。
ドキュメントの変更があるたびに、レプリケーターによってフィルター機能が適用され、フィルターの基準が満たされた場合にのみドキュメントが同期されます。
このフィルターを、以下のようなデータモデリングと組み合わせて利用することが考えられます。

たとえば、ドキュメントをモデル化する際に、「ステータス」プロパティーを設けます。
アプリケーションは、このステータスプロパティーの値を操作することによって、そのドキュメントを同期するタイミングを制御することができます。

あるいは、「スコープ」プロパティーを設け、リモートデータベースと同期する必要のないドキュメントでは、その値を「ローカル」に設定し、ドキュメントを同期しないことを示すことが考えられます。
この場合、インスタンスレベルで分割するパターンと異なり、ドキュメント間の結合を行うことができます。


=== プレビルドデータベース

アプリが利用するデータには、インストール時から変わらないデータもあれば、アプリの利用時期によってアップデートしなければならないデータも存在する場合があります。
さらに、そのアプリが使い始められるタイミングがエンドユーザーに委ねられているような場合、それらの区分が難しい場合があります。

//blankline

Couchbase Liteでは、あらかじめデータ登録済みのデータベース(プレビルドデータベース)をアプリケーションに同梱(バンドル)することができます。

アプリケーションを初めて利用する際に、サーバーから全てのデータをダウンロードするかわりに、プレビルドデータベースを利用することが考えられます。
プレビルドデータベースを使うことによって、このような初回データロードを省略、あるいは必要な範囲にのみ限定することができます。

最新化しなければならないデータの範囲があらかじめ明確に区別されていない場合であっても、プルレプリケーション実行により、リモートで更新されたデータのみが同期されます。
そのため、リリース時からの更新差分について外部的に管理する必要はありません。

====[column]デルタ同期

エンタープライズエディションでは、データ同期時にドキュメント内の変更があった部分のみを送信することのできるデルタ同期機能が提供されています。

エンタープライズエディションユーザーは、この機能を利用して、同期データサイズの最適化を行うことが可能です。

====[/column]

== ローカルデータの最適化

=== 同期済みデータのパージ

ローカルデータベースの肥大化を回避するために、またはコンプライアンス上の理由から、サーバーに同期された後にローカルデータベースからドキュメントを削除することが必要な場合があります。

//blankline

Couchbase Liteは、ドキュメントの同期ステータスについて通知を受けることのできる、イベント処理をサポートしています。
この機能を利用して、ドキュメントの同期ステータスに応じて、適切な処理を実行することができます。

たとえば、ドキュメントがリモートデータベースに対してプッシュされたことを示す@<tt>{onPushed}イベント受信時に、そのドキュメントをパージする処理を実装することが考えられます。

パージされたドキュメントの状態は、削除されたドキュメントとは異なり、リモートデータベースに同期されません。
そのため、リモートデータベースにのみドキュメントが格納されている状態になります。


=== ドキュメント有効期限

サーバー上で作成されたドキュメントには有効期限、つまりドキュメントがデータベース上に存続する期間であるTTL(Time To Live)が設定されている場合があります。

ネットワーク接続環境下で継続的に同期が行われている場合、リモートデータベースのドキュメントの状態がクライアント側に同期されるため、サーバー側で有効期限が切れて削除されたドキュメントは、いずれクライアント側でも削除されることになります。

ただし、クライアントがオフライン環境化で利用されることが予想され、そのような場合にも有効期限切れのドキュメントをタイムリーにクライアントから削除することが重要である場合があります。

//blankline

Couchbase Liteは、リモートデータベース上のドキュメントのTTLとは別に、ドキュメントに有効期限を設定する機能をサポートしています。
この機能を利用して、サーバーへの接続状態に関係なく、Couchbase Lite上のドキュメントを期限切れにすることができます。
Couchbase Liteで設定したドキュメントの有効期限が切れると、そのドキュメントはCouchbase Liteから自動的に削除されます。

Couchbase Liteで作成されたドキュメントについては、作成時に有効期限設定をすることができます。
一方、サーバーから同期されたドキュメントに対して有効期限を設定したい場合には、イベント処理を活用することができます。

具体的には、ドキュメントの@<tt>{Pull}イベント受信時に、同期されたドキュメントにTTLを設定します。
このTTLには、サーバーとクライアントとで同じ値を使うことも、あるいは要件に応じて別の値を使うこともできます。


//footnote[best-practices-common-patterns-couchbase-mobile-part1][https://blog.couchbase.com/best-practices-common-patterns-couchbase-mobile-part1/]
//footnote[best-practices-couchbase-mobile-database-sync-part2][https://blog.couchbase.com/best-practices-couchbase-mobile-database-sync-part2/]



