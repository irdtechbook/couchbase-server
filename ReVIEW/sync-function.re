
= Sync関数

Sync Gatewayでは、開発者がSync関数(Sync Function@<fn>{sync-func-doc})を定義することによって、データルーティングやアクセス制御などのアプリケーション固有の要件を実装します。

//footnote[sync-func-doc][https://docs.couchbase.com/sync-gateway/current/sync-function.html]

== 概要

=== 関数構造

Sync関数は、以下のような引数を取るJavaScript関数として定義されます。呼び出し元へ返す戻り値はありません。

//emlist[][JAVASCRIPT]{
function (doc, oldDoc, meta) { 

}
//}


引数は、それぞれ次のような意味を持ちます。

//blankline

 * @<tt>{doc}: ドキュメントのコンテンツを参照します。ユーザー定義のプロパティーに加え、以下のプロパティーを含みます。
 ** @<tt>{_id}: ドキュメントID
 ** @<tt>{_rev}: リビジョンID
 ** @<tt>{_deleted}: ドキュメント削除フラグ(ドキュメント削除の場合に含まれ、プロパティーの値は@<tt>{true}です)
 * @<tt>{oldDoc}: 更新によって置き換えられるリビジョンを参照します。ドキュメント新規作成の場合、nullになります。
 * @<tt>{meta}: ドキュメントのメタデータを参照します。


//blankline

Sync関数を実装する際には、JavaScript仕様に即して、パラメーターの記載を省略することができます。
@<tt>{meta}パラメーターをSync関数で利用しない場合に記載を省略することができ、@<tt>{meta}と@<tt>{oldDoc}の両方のパラメーターが必要ない場合も記載を省略できますが、@<tt>{oldDoc}パラメーターのみの省略はできません。


=== ドキュメント更新の制御

Sync関数定義においては、戻り値による制御は行われません。
その代わり、Sync関数内で例外を発生させることができ、例外の発生により関数の実行が中断された場合には、そのドキュメントの更新は実行されません。

開発者はSync Gateway関数を定義する際に、要件に応じた検証ロジックを実装し、検証に適わない場合には例外により処理を中断することによって、ドキュメントの更新を制御することができます。

このドキュメントの更新の制御は、ドキュメントの必須項目の検証の他、実行ユーザー権限の検証にも用いることができます。
Sync Gatewayのユーザーは、アクセスが許可されているチャネルに関連付けられたドキュメントにのみアクセスできます。
このチャネルへのアクセス権付与は、アクセス権限の種類(読み取り、書き込み)については区別しません。
つまり、ドキュメントにアクセスできるユーザーは誰でも、ドキュメントへの読み取りアクセスを持つ(Sync Gatewayからドキュメントをプルする)ことに加え、ドキュメントを更新する(Sync Gatewayに対してドキュメントをプッシュする)ことができます。
ドキュメントの書き込みアクセス制御は、Sync関数内で検証のプロセスを実装することによって実現されます。


== アクセス制御設定API

Sync関数を定義する際に開発者が利用することのできるAPI(Sync Function API@<fn>{sync-function-api})から、アクセス制御設定に関するものを紹介します。


//footnote[sync-function-api][https://docs.couchbase.com/sync-gateway/current/sync-function-api.html]

=== access

ユーザーにチャネルへのアクセスを許可します。

//blankline

次のふたつの引数を取ります。

 * ユーザーやロールを識別する文字列、またはその配列
 * チャネルを識別する文字列、またはその配列

//blankline

この関数をユーザーではなくロールに適用するには、ロール名の前に@<tt>{role:}を付けます。

オールチャネルワイルドカード（@<tt>{'*'}）を使用して、すべてのチャネルへのアクセスをユーザーに許可することができます。

ユーザーとロールは、管理者が明示的に作成する必要があります。

//blankline

次の例は、ユーザーにチャネルのアクセスを許可します。


//emlist[][JAVASCRIPT]{
access ("jchris", "mtv"); 
//}


次の例は、ユーザーに複数のチャネルへのアクセスを許可します。


//emlist[][JAVASCRIPT]{
access ("jchris", ["mtv", "mtv2", "vh1"]); 
//}


次の例は、複数のユーザーとロールにチャネルのアクセスを許可します。


//emlist[][JAVASCRIPT]{
access (["snej", "jchris", "role:admin"], "vh1");
//}

=== channel

ドキュメントをチャネルに割り当てます。

//blankline

次の引数を取ります。

 * チャネル名を識別する文字列、またはその配列

//blankline

指定するチャネルは、必ずしも事前に定義されている必要はありません。
ドキュメントがチャネルに割り当てられると、そのチャネルが存在していなかった場合には、暗黙的に作成されます。

有効なチャネル名は、テキスト文字（@<tt>{[A-Z、a-z]}）、数字（@<tt>{[0-9]}）、およびいくつかの特殊文字（@<tt>{[ = + / . , _ @]}）で構成されます。チャネル名では大文字と小文字が区別されます。

@<tt>{null}、または@<tt>{undefined}として解決される引数で@<tt>{channel}を呼び出すことが許容されています。
この場合、単に何も起こりません。
これにより@<tt>{channel(doc.channels)}のような使い方をするときに、@<tt>{doc.channels}が存在するかどうかを確認しなくても利用することが可能になっています。

以下の例は、ドキュメントの「published」プロパティーが真の場合、ドキュメントの「channels」プロパティーに定義されているチャネルに割り当てます。


//emlist[][JAVASCRIPT]{
if (doc.published) {
    channel(doc.channels);
}
//}

=== role

ユーザーにロールを追加します。

//blankline


次のふたつの引数を取ります。

 * ロールを識別する文字列、またはその配列
 * ユーザーを識別する文字列、またはその配列

//blankline

ロール名の前には@<tt>{role:}を付ける必要があります。

ユーザーとロールは、管理者が明示的に作成する必要があります。
存在しないロールはエラーを引き起こしませんが、ユーザーのアクセス権限には影響しません。
ロールの作成は、遡及的に行うことができます。存在しないロールをユーザーに追加した場合、事後的にロールが作成されるとすぐに、そのロールへの参照が有効になります。


//blankline


次の例は「owner」ロールをユーザーに割り当てます。


//emlist[][JAVASCRIPT]{
role("jchris", "role:owner"); 
//}


次の例は、指定されたふたつのロールをユーザーに割り当てます。


//emlist[][JAVASCRIPT]{
role("jchris", ["role:owner", "role:creator"]); 
//}


次の例は、「owner」ロールを、すべての指定されたユーザーに割り当てます。


//emlist[][JAVASCRIPT]{
role(["snej", "jchris", "traun"], "role:owner"); 
//}


== ドキュメント属性設定API

Sync関数を定義する際に開発者が利用することのできるAPI(Sync Function API@<fn>{sync-function-api})から、ドキュメント属性設定に関するものを紹介します。

=== expiry


ドキュメントの有効期限値(TTL)を設定します。

//blankline

次の引数を取ります。

 * ISO-8601形式の日付文字列、またはユニックスタイムで指定された有効期限値

//blankline

TTLが30日未満の場合は、現在の時刻からの秒単位の間隔として表すこともできます(たとえば、「5」を指定すると、ドキュメントが書き込まれてから5秒後に削除されます)。


//blankline

次の例は、ISO-8601形式の日付文字列を使用して、有効期限を設定しています。

//emlist[][JAVASCRIPT]{
expiry("2022-06-23T05:00:00+01:00") 
//}

== 権限検証API

Sync関数を定義する際に開発者が利用することのできるAPI(Sync Function API@<fn>{sync-function-api})から、権限検証に関するものを紹介します。

このタイプに属する関数は、権限の検証に際して、例外をスローすることで拒否を通知します。
その場合、Sync関数のそれ以降のコードは実行されず、ドキュメントの更新は行われません。

=== requireAccess

指定されたチャネル(の少なくともひとつ)にアクセスできるユーザーによって、行われていないドキュメントの更新を拒否します。

//blankline

次の引数を取ります。

 * チャネル名を識別する文字列、またはその配列

//blankline

@<tt>{requireAccess}は、具体的なチャネル名による許可のみを認識し、ワイルドカードを認識しません。
ユーザーがワイルドカード（@<tt>{*}）によって、すべてのチャネルに対するアクセスを許可されている場合、そのユーザーは@<tt>{requireAccess}の引数として指定される具体的なチャネルへのアクセスを明示的に許可されていないため、この関数呼び出しは失敗します。


//blankline

次の例は、ユーザーが「events」チャネルを読み取るためのアクセス権を持っていない限り、例外をスローします。


//emlist[][JAVASCRIPT]{
requireAccess("events"); 
//}

次の例は、ユーザーがドキュメントの@<tt>{channels}プロパティーに定義されているチャネル(の少なくともひとつ)にアクセスできない場合、例外をスローします。

//emlist[][JAVASCRIPT]{
requireAccess(oldDoc.channels); 
//}

=== requireAdmin


Sync Gateway 管理REST APIによって行われたものではないドキュメントの更新を拒否します。


//blankline

引数はありません。


//blankline


次の例は、 このリクエストが管理REST APIに送信されたものでない限り、例外をスローします。


//emlist[][JAVASCRIPT]{
requireAdmin();
//}

=== requireRole

指定されたロール(の少なくともひとつ)を持っているユーザーによって、行われていないドキュメントの更新を拒否します。


//blankline

次の引数を取ります。


 * ロールを識別する文字列、またはその配列

//blankline


次の例は、ユーザーが「admin」ロールを持っていない限り、例外をスローします。


//emlist[][JAVASCRIPT]{
requireRole("admin"); 
//}


次の例は、ユーザーがこれらのロールの少なくともひとつを持っていない限り、例外をスローします。


//emlist[][JAVASCRIPT]{
requireRole(["writer", "editor"]); 
//}

=== requireUser


指定したユーザーによって行われていないドキュメントの更新を拒否します。

//blankline

次の引数を取ります。


 * ユーザーを識別する文字列、またはその配列

//blankline


次の例は、ユーザーが「jchris」でない場合は例外をスローします。


//emlist[][JAVASCRIPT]{
requireUser("jchris"); 
//}


次の例は、ユーザーの名前がリストにない場合は例外をスローします。


//emlist[][JAVASCRIPT]{
requireUser(["snej", "jchris", "tleyden"]); 
//}

次の例は、現在ドキュメントの更新を実行しているユーザーが、そのドキュメントに所有者として記録されているユーザーであるかどうかを確認することによって、ドキュメントの所有者のみに変更を許可するロジックを示しています。


//emlist[][JAVASCRIPT]{
if (oldDoc) {
    requireUser(oldDoc.owner); 
}
//}


== 例外API

=== throw

(JavaScript)例外を発生させます。関数の実行は停止されます。

//blankline

後掲の例のようなメッセージを引数に指定することができます。

//blankline

たとえば、以下のような用途に利用することが考えられます。

//blankline


 * 特定のユーザーや、実行コンテクスト以外でのドキュメントの変更を許可しないように、要件に応じて実装された検証プロセスに適っていない場合に、例外を発生させます。
 * ドキュメントに対して特定の構造を強制するために、必要な制約をチェックし、それらが満たされていない場合に例外を発生させます。たとえば、必須プロパティーや、充足していなければならないプロパティーの組み合わせのチャック等が考えられます。
 * 無効な更新を許可しないために、更新前ドキュメント(@<tt>{oldDoc})と更新後ドキュメント(@<tt>{doc})とを比較し、不適切な変更が行われている場合に例外を発生させます。例としては、「ドキュメント作成者」のような、ドキュメントの新規作成以降は変更を許さないプロパティーが考えられます。

//blankline


次のように、Sync関数内でドキュメントモデルの検証を実装し、事前定義されたデータモデルに従わないドキュメントを、@<tt>{throw}メソッドを使用して拒否することができます。

//emlist[][JAVASCRIPT]{
if (!doc.title || !doc.creator || !doc.channels || !doc.writers) { 
    throw({forbidden: "Missing required properties"});
} else if (doc.writers.length == 0) {
    throw({forbidden: "No writers"});
}
//}


上記の関数がコールされると、ドキュメントの更新は、HTTP 403 "Forbidden"エラーコードで拒否され、@<tt>{forbidden:}プロパティーの値がHTTPステータスメッセージになります。


//footnote[access-control-how-control-document-access][https://docs.couchbase.com/sync-gateway/current/access-control-how-control-document-access.html]



== Sync関数定義における考慮点

開発者がSync関数を定義する際に、考慮すべき内容を紹介します。

=== 検証に利用するパラメーターの選択

Sync関数のなかでドキュメントを検証するロジックを実装する際には、更新後のドキュメントを、つまり更新後ドキュメント情報を保持する@<tt>{doc}パラメータのすべてのプロパティーを、基本的に「信頼できないもの」として扱うべきだといえます。
これは関数は常に、その検証対象とするデータへの変更が破壊的なものでないこと、ないし悪意による改変が含まれていないことを前提とするべきではない、という考え方によります。

たとえば、以下のコードは一見問題ないように見えます。

//emlist[][JAVASCRIPT]{
function(doc, oldDoc, meta) { 
    requireUser(doc.owners);
}
//}

ただし、上記の考え方に基づくと、以下の表現が適切になります。

//emlist[][JAVASCRIPT]{
function(doc, oldDoc, meta) { 
    requireUser(oldDoc.owners);
}
//}

ドキュメント情報に基づくアクセス権限などの検証を実装する場合には、@<tt>{doc}ではなく、@<tt>{oldDoc}のプロパティーを利用することが、往々にして適当だといえます。

一方で、以下のように、実行ユーザーが更新後ドキュメントのプロパティーの値と一致することを検証するケースも考えられます。
このケースは、ドキュメントの内容に対する検証の一種であると考えることができます。

//emlist[][JAVASCRIPT]{
function(doc, oldDoc, meta) {
    if (oldDoc == null) {
        requireUser(doc.creator);
    } else {
        requireUser(doc.updator);	
    }
}
//}


=== ドキュメント削除時の分岐

Couchbase Mobileにおけるドキュメントの変更はリビジョンの追加として実現されています。
これは、ドキュメントの削除の場合も同様です。
削除されたドキュメントは、@<tt>{_deleted:true}プロパティーを持つ単なるリビジョンです。
そして削除されたドキュメントには、@<tt>{_deleted:true}以外のプロパティーは存在しません。

ドキュメントが削除されている場合には、ドキュメントに対する検証は実際上必要になる場面はないと思われるかもしれません。
ただし、Sync関数は、変更と削除の別を問わず呼び出されることに注意が必要です。
そのため、特にドキュメント削除時の処理を明示的に設ける要件がないケースでこそ注意する必要が生じます。

ドキュメント削除時の処理が必要ない場合であっても、Sync関数には、ドキュメントの削除時にコールされた場合に、ドキュメントの更新時のために実装されている検証ロジックをスキップするための条件分岐を設ける必要があります。

@<tt>{doc._deleted}プロパティーをチェックして、削除時には不要な更新時の検証をスキップすることによって、削除時には存在しないプロパティーへアクセスすることにより発生する意図しないエラーを回避します。


=== 書き込み権限検証とアプリケーション設計

Sync関数は、Couchbase LiteデータベースとCouchbase Serverとの同期(レプリケーション)時に、Sync Gateway側(サーバーサイド)で実行されます。

モバイルアプリケーションのユースケースを想定した場合、本質的には、ユーザーは自身が行ったデータ編集が有効かどうかについて知る必要があるといえます。
つまり、ユーザーがドキュメントの保存を実行したタイミングで、その操作が無効な操作である場合には、アプリケーションは、ユーザーに対して、操作がシステムに反映されなかったことを伝えるべきです。

その意味で、Sync関数内での書き込み権限の検証は、サーバーサイドでの整合性を保証するための(最終的な)手段であり、アプリケーション設計上、クライアントサイドでは、ユーザーがデータを保存するアクションをトリガーとして、操作の有効性に対する検証と結果の通知が行われるべきであると考えることができます。


なお、サーバーサイドでの検証の利点としては、あるユーザーやユーザーのグループ(ロール)に対して、サーバー側でアクセス権限を失効させることができる点があります。

== Sync関数実装

Sync関数に関する章の締めくくりとして、実際のプロジェクトを想定したSync関数の実装について見ていきます。

=== 要件定義

Sync関数定義を実装する際には、まず要件を決定することから始めます。
要件定義着手にあたって、たとえば以下のような観点が考えられます。

//blankline

 * どのドキュメントを処理するか?
 * どのユーザーがどのドキュメントにアクセスする必要があるか?
 * ドキュメントの作成、更新、削除にどのような制約を課すか?

//blankline

ここでは、次の要件の実装について見ていきます。

//blankline

 * ドキュメントは、必ず次のプロパティーを持つ： @<tt>{title}, @<tt>{creator}, @<tt>{channels}, @<tt>{writers}
 * @<tt>{editor}ロールを持つユーザーへのみドキュメントの編集(作成・変更・削除)を許可
 * ドキュメントの@<tt>{writers}プロパティーに登録されているユーザーへのみドキュメントの変更(削除を含む)を許可
 * ドキュメントの@<tt>{creator}プロパティーに登録されているユーザーによるドキュメント作成のみを許可(ドキュメント新規作成時に、実行ユーザーと@<tt>{creator}プロパティーに登録されているユーザーが一致している)
 * ドキュメントの@<tt>{creator}プロパティーが変更されない

=== サンプル実装

以下は、上記の要件を実装したSync関数定義です。

//emlist[][JAVASCRIPT]{
function(doc, oldDoc, meta) {
    if (doc._deleted) {
        // ドキュメント削除時の分岐
        // ロールの検証: editorロールを持つユーザーによる削除でない場合、同期を拒否する。
        requireRole("editor"); 
        // ユーザー権限の検証: writersプロパティーに定義されているユーザーによる更新でない場合、同期を拒否する。
        requireUser(oldDoc.writers); 
        // 後続のロジックをスキップ
        return;
    }

    // 必須プロパティーの検証
    if (!doc.title || !doc.creator || !doc.channels || !doc.writers) { 
        throw({forbidden: "Missing required properties"});
    } else if (doc.writers.length == 0) {
        throw({forbidden: "No writers"});
    }

    if (oldDoc == null) {
        // 新規ドキュメント作成時の分岐
        // ロールの検証: editorロールを持つユーザーによる作成でない場合、同期を拒否する。
        requireRole("editor"); 
        // ユーザー権限の検証: creatorプロパティーに定義されているユーザーと実行ユーザーが同一でない場合、同期を拒否する。
        requireUser(doc.creator)
    } else {
        // 既存ドキュメント更新時の分岐
        // ユーザー権限の検証: writersプロパティーに定義されているユーザーによる更新でない場合、同期を拒否する。
        requireUser(oldDoc.writers); 
        // 変更不可プロパティーの検証: creatorプロパティーが変更されている場合、同期を拒否する。
        if (doc.creator != oldDoc.creator) {
            throw({forbidden: "Can't change creator"}); 
        }
    }
}
//}

====[column]Sync Gatewayにおける拡張属性利用

Couchbase Serverはドキュメントに対して、拡張属性(Extended Attributes@<fn>{java-sdk-xattr})を設ける機能を提供しています。
開発者は拡張属性(XATTR)を使って、ドキュメントにユーザー定義のメタデータを設定することができます。

拡張属性は、通常のアプリケーションロジックではなく(この場合、ドキュメントプロパティーを利用するのが適当)、ライブラリーやフレームワークでの利用が想定されています。

エンタープライズエディションでは、Sync関数内でドキュメントの拡張属性にアクセスすることができます。
これによって、たとえばアクセス制御の目的で利用される、Sync関数専用のデータ項目を、ドキュメントのプロパティーを使用するよりも、安全かつ効率的に保持することができます。

拡張属性の利用には、情報がドキュメントのコンテンツから分離されるという設計上の利点があります。
加えて、拡張属性の更新は新しいリビジョンを作成しないため、情報を更新してもドキュメントの同期が発生しない、という実行効率上の利点があります。

詳細についてはドキュメント(Use Extended Attributes for Access Grants@<fn>{access-control-how-use-xattrs-for-access-grants})を参照ください。

====[/column]

//footnote[access-control-how-use-xattrs-for-access-grants][https://docs.couchbase.com/sync-gateway/current/access-control-how-use-xattrs-for-access-grants.html]
//footnote[java-sdk-xattr][https://docs.couchbase.com/java-sdk/current/concept-docs/xattr.html]
