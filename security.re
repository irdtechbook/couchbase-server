
= セキュリティ

 
== 主要なセキュリティ要素

=== 認証 (Authentication)

Couchbase Serverへのユーザ（理者、アプリケーションを含む）アクセスは、認証を介してのみ行うことができます。
ユーザーは、ローカルまたは外部のパスワードレジストリを使用して認証されます（ネイティブLDAP、saslauthd、およびPAM）。
認証には、資格情報をサーバーに直接渡すか、資格情報が埋め込まれているクライアント証明書を使用する方法があります。接続は、SCRAMとTLSを使用して保護できます。

=== 認可 (Authorization)

Couchbase Serverは、ロールベースのアクセス制御(RBAC)によって、ユーザーを、個別に割り当てられた役割に関連付けます。ロールは、特定のシステムリソースへのアクセスの程度を許可する権限のセットです。

これにより、アクセスしようとしているシステムリソース、および実行したい操作に基づいて、アクセスを許可または拒否できます。
ロールは、ユーザーおよびグループに対して割り当てることができます。実行時、ロールはユーザ認証を介してクライアントに付与されます。

====[column]エディションによる差異
コミュニティエディションにおけるロールの位置づけは、ビルトインロールの提供に留まっており、本格的なロールベースのアクセス制御(RBAC)とは異なります。
また、コミュニティエディションには、ユーザーをグループ化する機能はなく、ロールはユーザーのみに割り当てることができます。

====[/column]

=== 監査 (Auditing)

Couchbase Serverで実行されるアクションを監査することを可能にします。
アクションのレビューにより、管理者はシステム管理タスクが適切に実行されていることを確認できます。

=== 暗号化 (Encription)

Couchbase Serverが管理するデータは、適切な復号化手段を所有する許可された関係者による場合を除き、読み取り不可能になるようにエンコードされます。そのため、データを安全に保存または送信できます。
これにより、ユーザーデータのプライバシー、およびサーバーとそのクライアントの整合性が保証されます。

== セキュリティ管理

Couchbase Serverにおいて、セキュリティのために管理可能な領域について紹介します。

=== ネットワークアクセス

暗号スイート、TLSレベル、およびコンソールアクセスを、個別に管理することが可能です。
また、クラスターに対するアクセスのみではなく、クラスタを構成するノード間のネットワーク通信の保護も可能です。

=== 証明書

証明書を、クラスターに対して定義・登録できます。
加えて、クライアントによって提示された証明書を許可することができます。

=== ログ

ログ中に、個人情報が含まれないように設定することが可能です。

=== セッション

ユーザーが一定期間非アクティブな状態が続いた後にセッションを終了するように構成することが可能です。

=== ポートマッピング

適切なファイヤーウォール設定のために、Couchbase Serverが利用する必要のある全てのポート番号の情報が公開されています。
これらのポートの多くについて、マッピングを変更することも可能です。



====[column]エディションによる差異
以下のセキュリティ関連機能は、エンタープライズエディションの機能として提供されています。

 * ロールベースアクセスコントロール(RBAC)
 * LDAP連携
 * ネットワークアクセス暗号化
 * x.509証明書
 * 監査
 * ログリダクション
 * ノード間暗号化
 * サイファー管理

====[/column]



=== 参考情報

Couchbase公式ドキュメント Security@<fn>{security-overview}

Couchbase公式ドキュメント Manage Security@<fn>{security-management-overview.}

Couchbase公式ドキュメント Network and Firewall Requirements@<fn>{install-ports}

Couchbaseブログ Data encryption in Couchbase Server@<fn>{data-encryption-in-couchbase-server}

GluuFederation Githubリポジトリ Couchbase should not listen by default on all server IPs@<fn>{issuecomment-402791618}



//footnote[security-overview][https://docs.couchbase.com/server/current/learn/security/security-overview.html]


//footnote[security-management-overview.][https://docs.couchbase.com/server/current/manage/manage-security/security-management-overview.html]

//footnote[install-ports][https://docs.couchbase.com/server/current/install/install-ports.html]

//footnote[data-encryption-in-couchbase-server][https://blog.couchbase.com/data-encryption-in-couchbase-server/]

//footnote[issuecomment-402791618][https://github.com/GluuFederation/community-edition-setup/issues/452#issuecomment-402791618]




== 認証 Authentication

Couchbase Serverにアクセスするには、ユーザーが認証されている必要があります。認証（Authentication）は、誰がシステムにアクセスしようとしているのかを識別するためのプロセスです。認証が成功した後、承認（Authorization）を実行できます。これにより、ユーザーの適切なアクセスレベルが決定されます。

Couchbase Serverの認証は通常、ユーザー名とパスワードに依存します。これらの情報は、アクセスを試みるユーザー（つまり、管理者またはアプリケーション）からシステムに渡される必要があります。
ユーザー名とパスワードは、事前に定義されているものと一致する必要があります。事前の定義情報は、Couchbase Serverクラスター自体、または外部システム、のいずれかで管理されます 。
また、クライアントアプリケーションから、x.509証明書を使用してCouchbase Serverに資格情報を渡すこともできます。

=== ビルトイン管理者アカウント

まず、Couchbaseクラスターの初期化時に、ユーザーがビルトイン管理者アカウントのユーザ名（デフォルトでは、@<tt>{Administrator}）とパスワードを設定します。その後、任意の数のユーザーをクラスターにいつでも追加できます。
追加ユーザーを管理者権限を持つユーザーとして作成することは可能ですが、ビルトイン管理者アカウントと、追加されたユーザーとでは、管理方法が異なります。ビルトイン管理者アカウントのパスワードを変更する際には、サーバー上で下記のコマンド(@<tt>{reset-admin-password}サブコマンドによる @<tt>{couchbase-cli} )を実行する必要があります。

//emlist{
couchbase-cli reset-admin-password [--regenerate] [--new-password <password>]
[--port <port>]
//}

=== 外部アクセシビリティ管理

ユーザーは、パスワードを指定する必要のない外部ユーザーとしてクラスターに追加することもできます。外部ユーザーは外部システムで認証されます。
外部システムによるアクセシビリティ管理としては、次のいずれかが用いられます。

 * LDAP（ライトウェイト・ディレクトリ・アクセス・プロトコル）
 * PAM（プラガブル認証モジュール）フレームワーク

====[column]エディションによる差異
LDAP、PAMを用いた管理は、Couchbase Serverのエンタープライズエディションでのみ利用可能です。

====[/column]

=== Webコンソールでの認証

Couchbase Webコンソールへのログインがデフォルトのポート8091によるhttpアクセスで実行されている場合、指定されたユーザー名とパスワードが平文で渡されます。

必要に応じて、セキュアなアクセス（ポート18091によるhttpsアクセス）を利用するように構成し、ユーザー名とパスワードが暗号化された形式で渡されるようにすることができます。

=== アプリケーションの認証

アプリケーションから資格情報を渡すには、Simple Authentication and Security Layer（SASL）フレームワークによって提供される4つのメカニズムのいずれかを使用する必要があります。SCRAMメカニズムにより、保護された形式でのみパスワードを送信することにより、アプリケーションは安全に認証できます。SHAベースのハッシュ関数をサポートするには、ドライバーの更新が必要になる場合があります。

このパスワード認証メカニズムには、次の方式があります（強度による昇順）。

 * PLAIN：クライアントは暗号化されていない形式でパスワードを送信します。すべてのクライアントがこの認証方法をサポートしています。これは安全ではなく、送信中にパスワードが盗まれるのを防ぐことはできません。
 * SCRAM-SHA1：160ビットのキーを使用します。
 * SCRAM-SHA256：SHA2と呼ばれるハッシュ関数のグループの1つである、256ビットのキーを使用します。
 * SCRAM-SHA512：SHA2グループの別のハッシュ関数であるSCRAM-SHA512は、512ビットのキーを使用します。サポートされている最も強力な認証プロトコルです。

SCRAM SHA 512が、Couchbase SDKの標準の認証方式です。
SCRAMは、Salted Challenge Response Authentication Mechanismの略称です。
これによって、「burute force」攻撃や、「man in the middle」攻撃を防ぐことができます。

最初のクライアントサーバーネゴシエーションでは、クライアント(アプリケーション)とサーバー（Couchbase Server）のOSの両方のOSでサポートされている最も強力な認証プロトコルが選択されます。たとえば、クライアントがPLAINプロトコルのみをサポートしている場合、PLAINプロトコルが使用されます。ただし、クライアントがSCRAM-SHA1プロトコルもサポートしている場合は、SCRAM-SHA1が使用されます。

チャレンジレスポンス方式は、暗号化されたチャネルと暗号化されていないチャネルの両方を介して送信できます。

SCRAMチャレンジ/レスポンスプロトコルは、パスワード検証のプロセスのみを認証します。後続のセッションを保護するには、TLSを使用する必要があります。

=== 証明書ベースの認証

Couchbase Serverは、クライアント認証のためのx.509証明書の使用をサポートしています。

証明書ベースの認証は、認証局（CA）に依存してIDを検証し、証明書を発行します。証明書には、識別したエンティティの名前、有効期限、証明書を発行したCAの名前、発行したCAのデジタル署名などの情報が含まれています。




=== パスワードポリシー設定

下記のようにCLIを使って、パスワードポリシーを変更できます。

//emlist{
couchbase-cli setting-password-policy
--cluster http://localhost:8091 -u Administrator
–p password --set --min-length 8 --uppercase
--lowercase --digits --special-chars
//}

デフォルトのポリシーは、以下の通りです。

//emlist{
{
"enforceDigits ": false ,
"enforceLowercase ": false ,
"enforceSpecialChars ": false ,
"enforceUppercase ": false ,
"minLength ": 6
}
//}

=== 参考情報

Couchbase公式ドキュメント Certificates@<fn>{security_certificates}

Couchbase公式ドキュメント Manage Certificates@<fn>{manage-certificates}

Couchbase公式ドキュメント Configureing saslauthd@<fn>{configure-saslauthd}

//footnote[security_certificates][https://docs.couchbase.com/server/current/learn/security/certificates.html]

//footnote[manage-certificates][https://docs.couchbase.com/server/current/manage/manage-security/manage-certificates.html]

//footnote[configure-saslauthd][https://docs.couchbase.com/server/current/manage/manage-security/configure-saslauthd.html]

== 認可 Authorization

=== 基本的なユースケース

ロールの基本的なユースケースを考えるに当たって、一般にアプリケーションのバックエンドとして使われるデータベースのユーザーは、管理者、アプリケーション、そして開発者に分類し、それぞれの特徴を整理します。

 * @<strong>{管理者}は、（Webコンソールにログインして）管理タスクを実行します。基本的に、データの読み取りまたは書き込みを行う必要はありません。
 * @<strong>{アプリケーション}は、データの書き込み、および（または）、読み取りを行います。Webコンソールにログインしたり、クラスターの設定を変更する必要はありません。
 * @<strong>{開発者}は、自分の担当するアプリケーションに関係するデータを参照する必要があり、多くの場合データの書き込み権限も必要になります。また、必要に応じWEBコンソールにログインし、クラスターの情報を参照したり、データを確認・操作することも必要とされるでしょう。


=== ビルトインロール

 * @<strong>{Full Admin}: Full Adminロールは、Couchbase Serverの全ての機能やリソースへの完全なアクセスを許可します。
 * @<strong>{Read-only Admin}: Read-only Adminロールは、Couchbase　Webコンソールへのアクセスを許可します。
 * @<strong>{Application Access}: 特定のバケットに対して、全ての権限を許可します。

====[column]エディションによる差異
エンタープライズエディションでは、リソース（スコープやコレクション、あるいは各サービス）および操作（データに対する読み取り、書き込み等）の両面で、詳細なロールベースのアクセスコントロール(RBAC)を利用可能です。

====[/column]


=== 参考情報

Couchbase公式ドキュメント Roles@<fn>{9d33fd7129234e2344d00e94d4853f2b}

Couchbase公式ドキュメント Role-Based Access Control (RBAC)@<fn>{0a0c84c83dc0992930bc018d8df1b279}

//footnote[9d33fd7129234e2344d00e94d4853f2b][https://docs.couchbase.com/server/current/learn/security/roles.html]

//footnote[0a0c84c83dc0992930bc018d8df1b279][https://docs.couchbase.com/server/current/rest-api/rbac.html]


== 暗号化 Encription


=== 通信データ(On the Wire)

TLSが利用できます（クライアント・サーバー、管理者アクセス、およびセキュアXDCR）。
また、TLSのためのX.509 certificatesの利用をサポートしています。

=== 永続化データ(At Rest)

サードパーティーベンダー(Vormetric, Protegrity, SafeNet)によって提供されます。

=== JSONドキュメントフィールド

SDKを用いた暗号化をサポートしています。