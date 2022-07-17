
= Sync Gateway概要

本章から、Couchbase Mobileの構成要素のひとつであるSyng Gatewayを扱います。

ここではまず、Sync Gatewayを理解するために必要な基本的な概念を解説します。

== データ同期

Sync Gatewayは、Couchbase LiteとCouchbase Serverとの間でデータ同期を行う機能を提供します。

//footnote[sync-using-app][https://docs.couchbase.com/sync-gateway/current/sync-using-app.html]

=== レプリケーション

レプリケーション(Replication@<fn>{wiki-replication})とは、字義通りには複製(Replica)を作ることといえますが、コンピューティングの分野でデータレプリケーションといった場合には、一般に複数の環境に同じデータを保存することを指します。
その具体的な内容は多岐に渡ります。

Couchbase Mobileについて「レプリケーション」という用語が用いられる場合、それは複数のCouchbaseデータベース間の自動データ同期を指しています。@<fn>{memo-replication-meaning}

//footnote[memo-replication-meaning][Couchbase Mobileの文脈で、「レプリケーション」という用語が用いられるときには、Couchbase LiteとCouchbase ServerとのSync Gatewayを介した同期を指すことが大部分です。ただし、後述のSync Gateway間レプリケーションにおいては、Sync Gatewayを介したCouchbase Serverクラスター間のデータ同期を指すなど、必ずしもそれだけの意味に止まらないため、ここでは「Couchbaseデータベース間」という表現が選ばれています。]

//footnote[wiki-replication][https://en.wikipedia.org/wiki/Replication_(computing)]


=== データ同期方向の種類

Sync GatewayとCouchbase Liteとの同期(Sync with Couchbase Lite@<fn>{sync-using-app})を実行するにあたって、データの同期方向に関して、以下の３通りからひとつを選択します。

//blankline

 * @<em>{プッシュ}: Couchbase Liteから、Couchbase Serverに対してデータの変更を同期します。
 * @<em>{プル}: Couchbase Liteへ、Couchbase Serverのデータの変更を同期します。
 * @<em>{プッシュアンドプル}: プッシュとプルの両方を行います。

//blankline

たとえば、アプリケーションのマスターデータのように、ユーザーによって変更されないデータについては、プルレプリケーションを使うことが考えられます。
また、単にアプリケーションからデータの収集を行うケース、たとえばエッジ端末上のセンサーデータを収集するIoTアプリケーションや、端末上でデータ登録を行うアプリケーションでは、プッシュレプリケーションを使うことができます。
どの端末でも、同じデータを参照する必要があり、端末上でユーザーがデータを作成・編集する場合には、プッシュアンドプルを使うことになるでしょう。

=== レプリケーション処理の種類

レプリケーションは、データの同期方向がプッシュであれプルであれ、Couchbase LiteアプリケーションがSync Gatewayに対して接続し、レプリケーションのリクエストを行うところから始まります。

Couchbase Liteにおけるレプリケーション処理の選択肢として、リクエスト時点で生じている全ての差分の同期が完了した後に処理を終了する@<em>{ワンショットレプリケーション}と、一旦すべての差分の同期が完了した後も継続して、以降のレプリケーションのために接続を続ける@<em>{継続的レプリケーション}とがあります。

アプリケーションは、ローカルデータベースとリモートデータベース間の同期を維持する場合、要件に応じて、継続的レプリケーションを利用することも、ワンショットレプリケーションをアプリケーションから定期的に実行することもできます。

=== リビジョン

複数のユーザーから同時に処理要求が行われる場合、データの一貫性を保証するために、データの複数のバージョンを制御する必要があります。
このような制御は、Multi Version Concurrency Control(MVCC)@<fn>{wiki-Multiversion_concurrency_control}と呼ばれます。

//footnote[wiki-Multiversion_concurrency_control][https://en.wikipedia.org/wiki/Multiversion_concurrency_control]


Sync Gatewayは、複数のCouchbase Liteアプリケーション(Sync Gatewayクライアント)で、同じデータに対する更新が発生する状況に対応する必要があります。
データが複数のデータベースにコピーされて管理されている状況では、あるクライアントの変更が、別のクライアントに対して反映される前に、そのクライアントが同じデータへの変更を行うことが起こりえます。
このような状況では、競合(データ更新の不整合)が発生します。

こうした状況に対応するために、Couchbase Mobileでは、ドキュメントが作成、更新、または削除されるたびに新たに@<em>{リビジョン}が作成されます。
「リビジョン」という用語について、GitやSubversionのような分散バージョン管理システムを利用したことのある人には特に説明を要しないと思われますが、「リビジョン」は「バージョン(番号)」のようなものと考えることができます。
ソフトウェアのバージョンのように(ソフトウェア会社によって)一元的に管理されているケースでは、バージョン番号は直線的に増加するのに対して、ある特定のバージョンに対して、複数の異なる環境(開発者やデータベース)で異なる変更を加えられるケースでは、同じ親リビションを持った子リビジョンがツリー状に枝分かれする状態が生じます。

ソフトウェアバージョン管理においては、こうした競合の発生においては、開発者によってマージが行われることになります。
Couchbase Mobileでは、こうした異なるリビジョン間の競合発生時において、デフォルトのルールの適用による自動競合解決機能を提供されています。
そのため、Couchbase Mobileを利用するにあたって、特に基本的な機能を理解する段階においては、ユーザーにとってリビジョンを意識する場面はあまりありません。

一方で、リビジョンはCouchbase Mobileを理解する上で、依然として重要な概念だといえます。
運用やトラブルシューティングに際して、リビジョンに関する知識が必要があります。

また、Couchbase Mobileでは、カスタム競合解決ロジックを開発することもできます。
その際には、同じドキュメントの異なるリビジョン間の競合をどのように解決するかを開発者が設計・実装することになります。

== アクセス制御モデル

Sync Gatewayのアクセス制御モデル(Access Control Concepts@<fn>{access-control-concepts})について解説します。


//footnote[access-control-concepts][https://docs.couchbase.com/sync-gateway/current/access-control-concepts.html]

=== 概要

Sync Gatewayにおいて、ドキュメントへのアクセスは、@<em>{ユーザー}、@<em>{ロール}、@<em>{チャネル}という、3つのエンティティーによって管理されます。


//image[sg-concept][Sync Gatewayアクセス制御モデル概念図]{
//}

(図は、CouchbaseドキュメントAccess Control Conceptsより)

//blankline

チャネルはドキュメントに関連付けられたタグと見なすことができます。
チャネルは、ドキュメントを分類し、アクセス制御を実施するための基本になります。
ユーザーに対してチャネルへのアクセスが許可されることによって、ユーザーがアクセスできるドキュメントが決定されます。

ロールは、ユーザーを論理的にグループ化します。ユーザーと同様に、ロールに対しても、チャネルへのアクセス権限を与えることができます。
ユーザーは、直接にまたはロールを介して割り当てられたチャネルにあるドキュメントのみにアクセスすることができます。


なお、このアクセス制御モデルは、ドキュメントに対するアクセス権限の種類(読み取りと書き込み)については区別しません。@<fn>{note-sync-func}


//footnote[note-sync-func][書き込み権限を制御する際には、後に紹介する、Sync関数を用いてアクセス制御を実装することができます。]

=== 利用例

チャネルを中心としたアクセス制御について、具体的に見ていきます。

まずは次の図を見てください。


//image[sg-usecase][Sync Gatewayアクセス制御利用例]{
//}

(図は、CouchbaseドキュメントAccess Control Conceptsより)

//blankline

ここでは、特定のユーザー(グループ)のみがアクセスできるドキュメント群と、全てのユーザーに共有されるドキュメント群が存在しています。

例として、小売チェーンの各店舗毎にユーザーまたはロールを割り当てるケースが考えられます。
各店舗向けに、在庫や営業スケジュールなど店舗固有の情報のためのチャネルがあります。そして、製品カタログのような小売チェーン店舗全体で共通の情報のためのチャネルがあり、すべての店舗は、この共有されたチャネルにアクセスします。



=== チャネル

チャネルエンティティーは、名前とドキュメントのリストで構成されます。

チャネルによって、ユーザー間でドキュメントを共有することができます。


チャネルには、ドキュメントのリストが割り当てられます。さらに、ユーザーまたはロールに対して、複数のチャネルへのアクセスを許可することができます。つまり、ユーザーとロールは許可されたチャネルのリストを持ちます。
この二重のリスト構造によって、以下のような目的が達成されます。

//blankline

 * ドキュメントをチャネルに割り当てることによる@<strong>{ドキュメントルーティング}
 * ユーザーにチャネルへのアクセスを許可することによる@<strong>{アクセス制御}

//blankline

具体的には次のことが実現可能になります。

//blankline

 * 多数のドキュメントをデータセットとして分割して管理する。
 * ユーザーに対して必要なドキュメントだけを公開する。
 * モバイルデバイスに同期されるデータの量を最小限に抑える。


//blankline


チャネルには、アプリケーションにより設定される@<em>{プライベートチャネル}と、あらかじめ定義されている@<em>{システムチャネル}とがあります。

以下のように、Sync Gatewayによって予め定義されているチャネルを、目的に応じて利用することができます。


@<em>{パブリックチャネル}（記号「@<tt>{!}」で表現されます）は、誰でも利用可能なドキュメントのためのチャネルです。
ドキュメントをユーザー全体で利用したい場合、このチャネルに割り当てます。ユーザーに明示的にパブリックチャネルへのアクセスを割り当てる必要はありません。全てのユーザーに対して、パブリックチャネルへのアクセスが自動的に許可されます。


@<em>{オールチャネルワイルドカード}（記号「@<tt>{*}」で表現されます）は、ユーザーに対して全てのチャネルへのアクセスを許可するときに使用することができます。
オールチャネルワイルドカードを使ってアクセスを許可すると、プライベートチャネルを含め、全てのチャネルのすべてのドキュメントにアクセスすることができるようになります。




=== ユーザー

ユーザーエンティティーは名前、パスワード、ロールのリスト、およびチャネルのリストで構成されます。

ユーザーは、アクセス制御の基本概念です。ドキュメントへのアクセスを特定のユーザーに制限することにより、アクセス制御を実施します。

チャネルへのアクセスを許可されたユーザーは、そのチャネルに割り当てられたすべてのドキュメントにアクセスできます。

ユーザーは、ロールに割り当てることもできます。ユーザーは、所属するすべてのロールのチャネルアクセスを継承します。


=== ロール

ロールエンティティーは、名前とチャネルのリストで構成されます。

ロールにより、同様の特性を持つユーザーをグループ化でき、ユーザーの管理が容易になります。

ロールに対して、チャネルへのアクセスが許可されます。ロールが割り当てられたすべてのユーザーは、ロールにアクセスが許可されているすべてのチャネル（そのチャネル内のドキュメント）にアクセスできます。

ロールに関連付けられているすべてのユーザーは、ロールのリスト内のチャネルにアクセスする権利を継承します。
これにより、複数のチャネルを複数のユーザーに関連付けることができます。

なお、ロールはユーザーとは別の名前空間で管理されるため、同じ名前のユーザーとロールを持つことができます。

== 機能と操作

=== REST API

Sync Gatewayに対して設定や操作を行う方法として、REST APIが提供されています。

Sync Gatewayには、下記の３種類のREST APIがあります。

//blankline

 * 管理REST API
 * メトリクスREST API
 * パブリックREST API

//blankline

管理REST APIは、Sync Gatewayの管理のための機能を提供します。

メトリクスREST APIは、Sync Gatewayシステム監視に用いられます。

パブリックREST APIは、Couchbase Lite以外のアプリケーションがSync Gatewayの機能を利用するために使うことができます。


各REST APIは、それぞれ異なるポート番号を使います。ポート番号を変更することも可能です。

構成方法等の詳細については、ドキュメント(Secure API Access@<fn>{rest-api-access})を参照ください。

//footnote[rest-api-access][https://docs.couchbase.com/sync-gateway/current/rest-api-access.html]


=== Sync関数

Sync Gatewayでは、アクセス制御モデルの適用のために、アプリケーション独自の実装を定義することができます。
これは、@<em>{Sync関数}(Sync Function@<fn>{sync-function-overview})と呼ばれるJavaScript関数を定義することによって実現されます。

Sync関数では、以下のような処理を制御することができます。

//blankline

 * チャネルへのドキュメントの割り当て
 * ユーザーやロールへのチャネルアクセスの許可
 * ユーザーの検証
 * ドキュメントの検証
 * 変更の非承認

//blankline

Sync関数は、ドキュメントに対して新しい更新が行われるたびに呼び出されます。

Sync関数内では、変更前と変更後のドキュメントの内容を参照することができます。
そのため、ドキュメント内容に対して検証を行うことができるだけでなく、ドキュメントの情報を利用してユーザーの検証などの操作を実現することが可能です。


//footnote[sync-function-overview][https://docs.couchbase.com/sync-gateway/current/sync-function-overview.html]


Sync関数内では、Sync Gatewayによって提供される様々な種類のヘルパー関数(Sync関数API)を利用することができます。

Sync関数の詳細については、後の章で解説します。

=== アクセス制御適用

Sync Gatewayにおけるドキュメントの配布やユーザーアクセスを構成する方法には、以下があります。

//blankline

 * 管理REST API
 * Sync関数

//blankline

構成管理の対象によって、これらの方法を使い分けることになります。


たとえば、以下の図に示すように、ユーザーやロールへのチャネルの追加は、両方の方法で構成可能です。
一方、ドキュメントのチャネルへの割り当てはSync関数を介して行います。

//image[configuration-points][アクセス制御適用概念図][]{
//}

(図は、CouchbaseドキュメントAccess Control Conceptsより)

//blankline

== Sync Gateway間レプリケーション

=== 概要

Sync Gatewayは、Sync Gateway間レプリケーション(Inter-Sync Gateway Replication@<fn>{sync-inter-syncgateway-overview})機能を提供します。
Sync Gateway間レプリケーションによって、異なるデータセンターにある複数のCouchbase Serverクラスター間の同期を行うことができます。

//footnote[sync-inter-syncgateway-overview][https://docs.couchbase.com/sync-gateway/current/sync-inter-syncgateway-overview.html]

Sync Gateway間レプリケーションは、多くの点でCouchbase LiteとSync Gatewayの間のレプリケーションと共通点を持っています。
たとえば、Sync Gateway間レプリケーションはWebSocketに基づいており、Couchbase Liteとのレプリケーションに使用されるものと共通のプロトコルが用いられます。

本書では、Sync Gateway間レプリケーションについての記述は、以下のユースケースの紹介に留めます。
構成管理方法等、Sync Gateway間レプリケーションの詳細についてはドキュメント@<fn>{sync-inter-syncgateway-overview}を参照ください。


=== クラウド/中央データセンターとエッジデータセンターの同期

このケースでは、複数のエッジデータセンターにあるCouchbase Serverクラスターがクラウドまたは中央データセンターのCouchbase Serverクラスターと同期します。
各エッジクラスターは、クラウドデータセンターへのネットワーク接続が失われた場合も自律的に動作します。

たとえば、AWS Wavelengthのようなサービスを活用し、キャリアネットワークにCouchbase Serverを配置して低遅延のアクセスを提供する際に、このような構成を取ることが考えられます。

//image[inter-sg-replication_1][クラウド/中央データセンターとエッジデータセンターの同期]{
//}

(図は、CouchbaseドキュメントInter-Sync Gateway Replicationより)

=== クラウド/データセンター間の同期

このケースでは、地理的に離れたCouchbase Serverクラスター同士が同期されます。


//image[inter-sg-replication_2][複数のクラウド/データセンター間の同期]{
//}

(図は、CouchbaseドキュメントInter-Sync Gateway Replicationより)

====[column]Couchbase ServerのXDCRとSync Gateway間レプリケーションの関係

Couchbase Serverは、XDCR(クロスデータセンターレプリケーション)という、複数のCouchbase Serverクラスター間のレプリケーション機能を提供しています。
Couchbase Mobileを利用しない場合、このXDCRを用いて、Couchbase Serverクラスター間のレプリケーションを行います。

Sync Gateway間レプリケーションを利用することによって、Couchbase LiteとCouchbase Serverとの同期を行うために、Sync Gatewayが管理する固有の情報を、Couchbase Serverクラスター間で同期することができます。
Couchbase Liteとの同期が介在する場合は、Couchbase Serverクラスター間の同期のためにSync Gateway間レプリケーションを用います。

====[/column]

