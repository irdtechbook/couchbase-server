
= セキュリティ

== 主要なセキュリティ要素

はじめに、Couchbase Serverにおけるセキュリティ要素の概観を示します。@<fn>{security-overview}

//footnote[security-overview][https://docs.couchbase.com/server/current/learn/security/security-overview.html]


=== 認証 (Authentication)

Couchbase Serverへのユーザー(管理者や、アプリケーション)からのアクセスは、認証を介してのみ行うことができます。
ユーザーは、ローカルまたは外部のパスワードレジストリを使用して認証されます。
認証には、資格情報をサーバーに直接渡す方法と、資格情報が埋め込まれているクライアント証明書を使用する方法があります。

=== 認可 (Authorization)

Couchbase Serverは、ロールベースのアクセス制御(RBAC)によって、ユーザーを、個別に割り当てられた役割に関連付けます。ロールは、特定のシステムリソースへの、特定の操作を許可する権限のセットです。
これにより、ユーザーがアクセスしようとしているシステムリソース、および実行しようとしている操作に応じて、システムはそのアクセスを許可または拒否します。

ロールは、ユーザー認証を介してクライアントに付与されます。

ロールは、ユーザーおよびグループに対して割り当てることができます。

====[column]エディションによる差異
コミュニティエディションには、ユーザーをグループ化する機能はなく、ロールはユーザーのみに割り当てることができます。

コミュニティエディションにおけるロールの位置づけは、限られたビルトインロールの提供に留まっており、本格的なロールベースのアクセス制御(RBAC)とは異なります。

====[/column]

=== 監査 (Auditing)

Couchbase Serverで実行されるアクション(発生するイベント)を監査(ログ出力)することが可能です。

監査イベントの種類については、リファレンス@<fn>{audit-event-reference}を参照することができます。

//footnote[audit-event-reference][https://docs.couchbase.com/server/current/audit-event-reference/audit-event-reference.html]

=== 暗号化 (Encription)

Couchbase Serverが管理するデータは、適切な復号化手段を所有する許可された関係者による場合を除き、読み取り不可能になるように暗号化することができます。
これによって、データを安全に保存または送信することができます。

== セキュリティ管理項目

Couchbase Serverのセキュリティを高めるために、管理可能な領域について紹介します。@<fn>{security-management-overview}

//footnote[security-management-overview][https://docs.couchbase.com/server/current/manage/manage-security/security-management-overview.html]

=== ネットワークアクセス

暗号スイート、TLSレベル、およびコンソールアクセスを、個別に管理することが可能です。

クラスターに対するアクセスのみではなく、クラスターを構成するノード間の通信の保護も可能です。

=== 証明書

クラスターに対して、証明書を登録できます。

クライアントによって提示された証明書を許可することができます。

=== ログリダクション

個人情報がログに含まれないようにすることが可能です。

=== セッションタイムアウト

一定期間非アクティブな状態が続いた後に、セッションを終了するように構成することが可能です。

=== ポートマッピング

適切なファイヤーウォール設定を行うために、Couchbase Serverが利用する必要のある全てのポート番号の情報が公開されています。@<fn>{install-ports}

これらのポートの多くについて、マッピングを変更することが可能です。

//footnote[install-ports][https://docs.couchbase.com/server/current/install/install-ports.html]

====[column]エディションによる差異
以下のセキュリティ関連機能は、エンタープライズエディションの機能として提供されています。

 * 外部アクセシビリティ管理
 * ロールベースアクセスコントロール(RBAC)
 * ネットワークアクセス暗号化
 * X.509証明書
 * 監査
 * ログリダクション
 * ノード間暗号化
 * サイファー管理

====[/column]




== 認証

認証（Authentication@<fn>{authentication}）は、誰がシステムにアクセスしようとしているのかを識別するためのプロセスです。Couchbase Serverにアクセスするには、認証される必要があります。


Couchbase Serverの認証は通常、ユーザー名とパスワードに依存します。これらの情報は、アクセスを試みるユーザー（管理者やアプリケーション）から渡される必要があります。
ユーザー名とパスワードは、事前に定義されているものと一致する必要があります。事前の定義情報は、Couchbase Serverクラスター、または外部の認証システムのいずれかで管理されます。

また、クライアントアプリケーションから、X.509証明書を使用してCouchbase Serverに資格情報を渡すこともできます。


//footnote[authentication][https://docs.couchbase.com/server/current/learn/security/authentication.html]

=== ビルトイン管理者アカウント

クラスターの初期化時に、ビルトイン管理者アカウントのユーザー名とパスワードを設定します(その後、任意の数のユーザーをクラスターにいつでも追加できます)。

ビルトイン管理者アカウントとは別に、管理者権限を持つユーザーを作成することは可能ですが、ビルトイン管理者アカウントとクラスター初期化後に追加作成されたユーザーとでは、管理方法が異なります。ビルトイン管理者アカウントのパスワードを変更する際には、サーバー上で下記のコマンドを実行する必要があります。

//emlist{
couchbase-cli reset-admin-password [--regenerate] [--new-password <password>]
[--port <port>]
//}

=== パスワードポリシー

Couchbase Serverが管理するパスワードには、パスワード強度のルールを定義するパスワードポリシーがあります。

//footnote[usernames-and-passwords-password-strengthd][https://docs.couchbase.com/server/current/learn/security/usernames-and-passwords.html#password-strengthd]

Couchbase Serverのパスワードポリシーを、CLIを使って変更することが可能です。@<fn>{couchbase-cli-setting-password-policy}

下記は、その実行例です。

//cmd{
$ couchbase-cli setting-password-policy \
  --cluster http://localhost:8091 \
  -u Administrator  -p password --set \
  --min-length 8 --uppercase 1 \
  --lowercase 1 --digit 1 --special-char 1
//}

@<tt>{--get}オプション指定により、現在のパスワードポリシーを確認することができます。デフォルトのポリシーは、以下の通りです。

//emlist{
{
"enforceDigits ": false ,
"enforceLowercase ": false ,
"enforceSpecialChars ": false ,
"enforceUppercase ": false ,
"minLength ": 6
}
//}

また、REST APIを使って、設定を行うことも可能です。@<fn>{rest-set-password-policy}

//footnote[couchbase-cli-setting-password-policy][https://docs.couchbase.com/server/current/cli/cbcli/couchbase-cli-setting-password-policy.html]

//footnote[rest-set-password-policy][https://docs.couchbase.com/server/current/rest-api/rest-set-password-policy.html]

=== 外部アクセシビリティ管理

外部システムで認証されるユーザーを、クラスターに追加することができます。外部ユーザーは外部システムで認証されるため、クラスターでは認証情報(パスワード)を管理しません。

外部システムによるアクセシビリティ管理としては、次のいずれかが用いられます。

 * LDAP (Lightweight Directory Access Protocol)
 * PAM (Pluggable Authentication Modules)

====[column]エディションによる差異
外部アクセシビリティ管理は、エンタープライズエディションでのみ利用可能です。

====[/column]

=== Webコンソールでの認証

Webコンソールへのログインがデフォルトのポート(8091)によるhttpアクセスで実行されている場合、ユーザー名とパスワードは、平文で渡されます。

必要に応じて、セキュアなアクセス(ポート18091によるhttpsアクセス)を利用するように構成し、ユーザー名とパスワードを暗号化された形式で渡すようにすることができます。

=== アプリケーションの認証

アプリケーションから資格情報を渡すには、SASL(Simple Authentication and Security Layer)フレームワークによって提供される4つのメカニズムのいずれかを使用します。

このパスワード認証メカニズムには、次の方式があります(強度による昇順で掲載)。

 * @<strong>{PLAIN} 暗号化されていない形式でパスワードを送信します。
 * @<strong>{SCRAM-SHA1} 160ビットのキーを使用します。
 * @<strong>{SCRAM-SHA256} SHA2と呼ばれるハッシュ関数グループの1つ。256ビットのキーを使用します。
 * @<strong>{SCRAM-SHA512} SHA2グループのハッシュ関数。512ビットのキーを使用します。サポートされている最も強力な認証プロトコルです。

SCRAM(Salted Challenge Response Authentication Mechanism)を用いて、保護された形式でパスワードを送信することにより、アプリケーションを安全に認証できます。

クライアント/サーバー間ネゴシエーションにより、クライアント(アプリケーション)とサーバー(Couchbase Server)の両方のOSでサポートされている最も強力な認証プロトコルが選択されます。
たとえば、クライアントがPLAINプロトコルのみをサポートしている場合、PLAINプロトコルが使用されます。クライアントがSCRAM-SHA1プロトコルをサポートしている場合は、SCRAM-SHA1が使用されます。

SCRAMチャレンジ/レスポンスプロトコルは、パスワード検証のプロセスのみを保護します。後続のセッションを保護するには、TLSを使用する必要があります。

=== 証明書ベースの認証

Couchbase Serverは、クライアント認証のためのX.509証明書の使用をサポートしています。@<fn>{security_certificates}


//footnote[security_certificates][https://docs.couchbase.com/server/current/learn/security/certificates.html]





== 認可

認証が成功した後、認可(Authorization@<fn>{authorization-overview})が行われます。認可(または承認と呼ばれる場合もあります)により、適切なアクセスレベルが決定されます。


//footnote[authorization-overview][https://docs.couchbase.com/server/current/learn/security/authorization-overview.html]

=== データベースユーザーの種類

ロール(Role@<fn>{9d33fd7129234e2344d00e94d4853f2b})について説明するにあたって、まず基本的なユースケースを整理します。
一般にアプリケーションのバックエンドとして使われるデータベースのユーザーは、管理者、開発者、アプリケーションに分類することができます。

 * @<strong>{管理者}は、クラスターの管理タスクを担います。Webコンソールにログインしたり、CLIを使って管理タスクを実行します。データの登録を担当することも考えられます。
 * @<strong>{開発者}は、データを参照したり、データの書き込みを行うためにデータベースにアクセスすることがあります。また、Webコンソールなどでクラスターの情報を参照する場合があります。
 * @<strong>{アプリケーション}は、データベースに対して、データの読み取りと書き込みを行います。

=== ビルトインロール

以下に、ビルトインロールの概要を記します。

 * @<strong>{Full Admin}ロールは、Couchbase Serverの全ての機能やリソースへの完全なアクセスを許可します。
 * @<strong>{Read-only Admin}ロールは、統計情報を含むクラスター管理情報の参照を許可します。また、Webコンソールへのアクセスを許可します。
 * @<strong>{Application Access}を介して、特定のバケットに対して、全ての権限を許可します。

====[column]エディションによる差異
エンタープライズエディションでは、よりきめ細かいロールベースのアクセスコントロール(RBAC)を利用可能です。

====[/column]



//footnote[9d33fd7129234e2344d00e94d4853f2b][https://docs.couchbase.com/server/current/learn/security/roles.html]

//footnote[0a0c84c83dc0992930bc018d8df1b279][https://docs.couchbase.com/server/current/rest-api/rbac.html]


== 暗号化

Couchbase Serverは、データの暗号化(Encryption@<fn>{encryption-overview})と復号化を、様々なレベルで広範囲にサポートします。

//footnote[encryption-overview][https://docs.couchbase.com/server/current/learn/security/encryption-overview.html]

=== 通信データ

通信データ(On the Wire)暗号化のために、TLSが利用できます。

通信データ暗号化の範囲は、クライアント/サーバー間、コンソールアクセス、ノード間、およびクラスター間(XDCR)をカバーします。

また、TLSのためのX.509証明書の利用をサポートしています。

=== 永続化データ

永続化データ(At Rest)暗号化の機能は、下記のようなサードパーティー暗号化ツールとの組み合わせにより実現されます。

 * Linux Unified Key Setup (LUKS)@<fn>{sec-Using_LUKS_Disk_Encryption}
 * Thales CipherTrust@<fn>{thales} (旧Vormetric/Gemalto)
 * Protegrity@<fn>{protegrity}

LUKSによる暗号化については、設定手順についてのドキュメント@<fn>{luks-encryption-procedure}とスクリプト@<fn>{create_luks_fs}が提供されています。

=== システム機密情報

データベースに登録されているパスワードや証明書のような、システム機密情報(System Secrets@<fn>{manage-system-secrets})は、暗号化されたフォーマットでディスクに保存されます。

他のデータベースには、ユーザーの情報をデータベース(システムテーブル)で管理し、参照することのできるものもありますが、Couchbase Serverは、それらとは異なり、ユーザー情報の管理・参照のために、システムテーブルは利用されていません。



//footnote[sec-Using_LUKS_Disk_Encryption][https://en.wikipedia.org/wiki/Linux_Unified_Key_Setup]

//footnote[luks-encryption-procedure][https://docs.couchbase.com/server/current/manage/manage-security/manage-connections-and-disks.html#luks-encryption-procedure]
//footnote[create_luks_fs][https://github.com/couchbase/perfrunner/blob/master/scripts/create_luks_fs.sh]



//footnote[thales][https://cpl.thalesgroup.com/encryption/transparent-encryption]

//footnote[protegrity][https://www.protegrity.com/]

//footnote[manage-system-secrets][https://docs.couchbase.com/server/current/manage/manage-security/manage-system-secrets.html]


=== JSONドキュメントフィールド

Couchbase Server Java SDKを使用して、JSONドキュメントの特定のフィールドを暗号化および復号化することができます。

フィールドレベルの暗号化(Field Level Encryption@<fn>{java-sdk-encryption})の操作方法として、次の2つの方法が利用可能です。

 * データバインディング中の透過的な暗号化/復号化
 * フィールドに対する直接的な操作


====[column]エディションによる差異
フィールドレベルの暗号化は、エンタープライズサブスクリプションライセンス契約により提供されるオプションのライブラリーとして実現されています。

====[/column]


//footnote[java-sdk-encryption][https://docs.couchbase.com/java-sdk/current/concept-docs/encryption.html]