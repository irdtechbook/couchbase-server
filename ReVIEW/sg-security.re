
= Sync Gatewayセキュリティ


Sync Gatewayへのアクセスに関するセキュリティ(Secure Sync Gateway Access@<fn>{secure-sgw-access})機能について解説します。@<fn>{memo-what-about}

//footnote[memo-what-about][ここでは、Sync Gatewayで行うセキュリティ構成について記述しています。Sync Gatewayに接続するCouchbase Liteアプリケーションで行うセキュリティ構成については、後のCouchbase Liteレプリケーションの章を参照ください。]

//footnote[secure-sgw-access][https://docs.couchbase.com/sync-gateway/current/secure-sgw-access.html]

== 概要


Sync Gatewayで発生する通信経路について整理すると、以下の種類があります。

//blankline

 * Sync Gatewayのクライアント(Couchbase Liteや、パブリックREST APIへアクセスするユーザー)との通信
 * Sync Gatewayの管理/監査ユーザー(管理/メトリクスREST APIへアクセスするユーザーやシステム)との通信
 * Couchbase Serverとの通信

//blankline

Sync GatewayとCouchbase Serverとの通信においては、Sync GatewayがCouchbase Serverのクライアントの位置づけとなります。
Sync Gatewayの通信におけるセキュリティを理解する際には、Sync Gatewayがサーバーの役割を果たす場合と、クライアントの役割を果たす場合とを区別することが重要になります。

例えば、Sync GatewayにおけるTLS対応という時、それは直接的にはまずサーバーとしてのセキュリティ機能として考えられますが、Sync GatewayがクライアントとしてCouchbase Serverへ接続する際の構成オプションとしての面も存在します。

== ユーザー認証

Sync Gatewayにおける、ユーザー認証(User Authentication@<fn>{authentication-users})について解説します。

ここでいう「ユーザー」とは、Sync GatewayのクライアントであるCouchbase Liteアプリケーションで用いられるSync Gatewayユーザー(Sync Gateway User@<fn>{lbl-sgw-users})を指します。

//footnote[authentication-users][https://docs.couchbase.com/sync-gateway/current/authentication-users.html]


=== 匿名アクセス

Sync Gatewayでは、「GUEST」という名前の付いた特別なユーザーアカウントが、認証されていないリクエストに適用されます。AuthorizationヘッダーまたはセッションCookieを持たないパブリックREST APIへのリクエストは、GUESTアカウントからのものとして扱われます。この匿名アクセスはデフォルトで無効になっています。

GUESTアカウントを有効化して利用する場合、GUESTアカウントに対して、特定のチャネルへのアクセスを許可します。チャネルをGUESTアカウントに割り当てない限り、匿名のリクエストはどのドキュメントにもアクセスできません。


=== 基本認証

Sync Gatewayユーザーを作成する際に、認証情報としてユーザー名とパスワードを定義します。
Couchbase Liteアプリケーションで、そのユーザー名/パスワードを使用して、Sync Gatewayに対して認証することができます。

レプリケーターは最初のリクエストで認証情報(クレデンシャル)を送信してセッションクッキーを取得し、これを後続のリクエストに使用します。

=== 認証プロバイダー

Sync Gatewayは、FacebookやGoogleのようなサードパーティー認証機関を利用するためのソリューションを提供します。

認証プロバイダーから提供されているSDKを利用してトークンを取得する責任はアプリケーションにあります。
アプリケーションは、取得したトークンをSync Gatewayに送信し、Sync GatewayからセッションIDを受け取ります。
このセッションIDが、その後のリクエストに使用されます。


次の図は、この一連の手順を示しています。


//image[user-auth_1][認証プロパイダー連携シーケンス]{
//}

(図は、CouchbaseドキュメントUser Authenticationより引用)

//blankline

上記図にあるように、Sync Gatewayは、下記のエンドポイントを提供します。


//blankline

 * @<tt>{/{db\}/_facebook}
 * @<tt>{/{db\}/_google}

//blankline

ドキュメントに掲載されている、パブリックREST APIのAPIエクスプローラー@<fn>{admin-rest-api}(Authenticationセクション)にて、利用方法を確認することができます。

//footnote[admin-rest-api][https://docs.couchbase.com/sync-gateway/current/rest-api.html#api-explorer]

=== カスタム認証

カスタム認証では、アプリケーションサーバーを用いて、認証のための独自のフロントエンドを設けます。
このアプリケーションサーバーでは、他の認証サービスと、Sync Gateway REST APIを組み合わせて認証プロセスを構築します。
Sync Gatewayは、そのために利用することのできる機能を提供します。

次の図は、Couchbase MobileアプリケーションでGoogleサインインとの連携を行うアーキテクチャの例を示しています。

詳細については、ドキュメント@<fn>{custom-authentication}を参照ください。

//image[user-auth_2][カスタム認証概念図]{
//}

(図は、CouchbaseドキュメントUser Authenticationより引用)


//footnote[custom-authentication][https://docs.couchbase.com/sync-gateway/current/authentication-users.html#custom-authentication]

=== OpenID Connect


Sync GatewayはOpenID Connectに対応しています。これにより、OpenID Connectをサポートするサードパーティ認証プロバイダーに認証を委任することができます。

詳細については、ドキュメント@<fn>{openid-connect}を参照ください。
また、OpenID Connect利用に関するチュートリアル「Set up an OpenID Connect authentication for the Sync Gateway@<fn>{openid-connect-implicit-flow}」が公開されています。


//footnote[openid-connect-implicit-flow][https://docs.couchbase.com/tutorials/openid-connect-implicit-flow/index.html]


//footnote[openid-connect][https://docs.couchbase.com/sync-gateway/current/authentication-users.html#openid-connect]

== TLS証明書認証

=== 概要

Sync Gatewayは、TLS証明書認証(TLS Certificate Authentication@<fn>{authentication-certs})をサポートします。

//footnote[authentication-certs][https://docs.couchbase.com/sync-gateway/current/authentication-certs.html]

TLS証明書認証を有効にするには、次の2つの構成プロパティを追加します。

//blankline

 * SSLCert: X.509証明書または証明書チェーンを含むPEM形式のファイルへのパス
 * SSLKey: 証明書と一致する秘密鍵を含むPEM形式のファイルへのパス

//blankline

これらのプロパティが存在する場合、Sync Gatewayインスタンスは、TLSのみに応答します。
HTTP接続とHTTPS接続の両方をサポートしたい場合は、2つの別々のSync Gatewayインスタンスを実行する必要があります。

=== 構成方法

TLS証明書を利用するには、信頼されている認証局(CA)から証明書を取得するか、自己署名証明書を作成します。
どちらの方法で証明書を準備した場合でも、秘密鍵とX.509証明書が得られます。

これらのファイルをSync Gatewayプロセスが読み取り可能なディレクトリに配置します。
その際には、許可されていないユーザーがファイルを読み取れないようにすることが重要です。



== REST APIアクセス

=== 認証と承認

REST APIへのアクセス制御の方法は、パブリックREST APIと、その他のAPI(管理REST APIとメトリクス REST API)とで異なります。

パブリックREST APIへのリクエストを送信するユーザーについては、Sync Gatewayユーザー(Sync Gateway User@<fn>{lbl-sgw-users})を作成し、Sync Gateway構成における適切な権限管理を行います。

一方、管理/メトリクスREST APIへのリクエストを送信するユーザーについては、Couchbase Serverのユーザー(RBAC Users@<fn>{lbl-rbac-users})を作成し、必要な権限のセットを与えます。


//footnote[lbl-sgw-users][https://docs.couchbase.com/sync-gateway/current/access-control-concepts.html#lbl-sgw-users]
//footnote[lbl-rbac-users][https://docs.couchbase.com/sync-gateway/current/access-control-concepts.html#lbl-rbac-users]

=== アクセス経路保護

Sync Gatewayでは、REST APIのエンドポイントをAPIの種類毎に構成します。それぞれに、デフォルトで異なるポート番号が割り当てられています。

管理REST APIやメトリクスREST APIは、通常内部ネットワークに公開されていれば十分だと考えられます。

一方、パブリックREST APIに対しては、ファイアウォールを構成する等、必要に応じ外部からの安全な接続を構成することが考えられます。

このように、APIの性格、利用法に応じて、ポート番号レベルで構成を行うことが想定されています。

== Couchbase Server接続

=== 概要

Sync GatewayからのCouchbase Serverへの接続に際して、次のふたつの方法があります。

//blankline

 * Couchbase Serverのユーザー名とパスワードを、Sync Gateway構成情報として指定します。
 * Couchbase Serverのルート証明書によって署名されたX.509クライアント証明書を使用します。これは、ユーザー名とパスワードを指定する方法の代替として、もしくは同時に使用することができます。

Sync GatewayからCouchbase Serverへの接続は、データベース(バケット)指定により構成されます。
従って、予めCouchbase Serverに該当するバケットが存在する必要があるのに加えて、Couchbase Serverへの接続に用いられるユーザーがそのバケットに対する適切な権限を有している必要があります。

//blankline

=== X.509クライアント証明書

Couchbase ServerとSync Gatewayとの間でX.509証明書ベースの認証を使用するには、はじめにCouchbase Serverでルート証明書とサーバ証明書を作成します。
ルート証明書とサーバ証明書によってCouchbase Serverクラスターを保護した後で、クライアント証明書をルート証明書で署名します。
このクライアント証明書を用いて、Sync Gatewayでクライアント証明書認証を有効にすることによって、証明書によって保護されたCouchbase serverへの接続を設定することができます。

Couchbase Serverクラスターのルート証明書によって承認されるクライアント証明書の作成方法については、Couchbase Serverドキュメント(Configure Client Certificates for Couchbase Server@<fn>{e21019312e8c5273523ecb706e8b76e8})を参照ください。

//footnote[e21019312e8c5273523ecb706e8b76e8][https://docs.couchbase.com/server/current/manage/manage-security/configure-client-certificates.html#client-certificate-authorized-by-a-root-certificate]

なお、Sync Gateway構成情報内に、Couchbase Serverのユーザー名/パスワードとX.509証明書に関するプロパティの両方が指定されている場合、パスワードベースの認証が行われた上で、TLSハンドシェイクの際にクライアント証明書が用いられます。

X.509証明書ベースの認証は、以下のSync Gateway構成プロパティに各ファイルへのパスを指定することによって有効になります。

//blankline

 * @<tt>{bootstrap.ca_cert_path}
 * @<tt>{bootstrap.x509_cert_path}
 * @<tt>{bootstrap.x509_key_path}

//blankline

構成プロパティの詳細については、ドキュメント(Bootstrap Configuration@<fn>{configuration-schema-bootstrap})を参照ください。


//footnote[configuration-schema-bootstrap][https://docs.couchbase.com/sync-gateway/current/configuration-schema-bootstrap.html]


適切と思われる構成を行なったにも関わらず接続が失敗する場合は、クラスター証明書エラー表(Cluster Certificate Errors@<fn>{handle-certificate-errors})を問題解決に役立てることができます。


//footnote[handle-certificate-errors][https://docs.couchbase.com/server/current/manage/manage-security/handle-certificate-errors.html#cluster-certificate-errors]


=== TLS無効化


テストや開発時などの簡略化のために、TLSを無効にする設定が提供されています。
@<tt>{bootstrap.server_tls_skip_verify}フラグを@<tt>{true}にを設定することによって、TLSを無効にすることができます。

TLS暗号化利用時には、接続先のCouchbase Serverを指定する接続文字列のプロトコルとして @<tt>{couchbases://}を指定します(@<tt>{https://}のように、「@<tt>{s}」で終わっていることに注意)。
TLSを無効にした場合は、接続文字列に @<tt>{couchbase://}を指定します。

====[column]Couchbase ServerとのTLS通信

Couchbase ServerではクライアントとのTLS通信はエンテープライズエディションにおける機能として位置付けられています。
一方、Sync GatewayではTLSの利用にエディションによる制約はなく、コミュニティエディションでも用いることができます。
この不整合は将来何らかの形で解消されるかもしれませんが、現時点においてはコミュニティエディションユーザーにとって混乱の原因となる可能性があるため記します。

====[/column]

