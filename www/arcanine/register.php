<?php

function prepare (string $sql) : PDOStatement 
{
  static $pdo;
  $pdo ??= new PDO('sqlsrv:Server=(local)\SQLEXPRESS;Database=soma', 'soma', 'soma');
  return $pdo->prepare($sql);
}

function affect (string $sql, array ...$args) : bool 
{ 
  return prepare($sql)->execute($args);
}

function retrieve (string $sql, array ...$args) : array 
{
  ($statement = prepare($sql))->execute($args);
  return $statement->fetchAll(PDO::FETCH_ASSOC);
}

function validations () : array 
{
  return [
    'You must fill out all fields, before submitting!' => array_filter($_POST, fn (string $val) => strlen($val) !== 0) !== $_POST,
    'Your email is entered incorrecty.' => !filter_var($_POST['Email'], FILTER_VALIDATE_EMAIL), 
    'Account already taken.' => retrieve('SELECT 1 FROM NGSCUSER WHERE strUserId = ? OR strEmail = ?', $_POST['Username'], $_POST['Email']),
    'Your Security code must be 5 digits long.' => strlen($_POST['wSerial']) < 5,
  ];
}

function response () : string 
{
  $errors = array_filter(validations());
  $messages = array_keys($errors); 
  $errors or (
    affect('INSERT INTO NGSCUSER(strUserId,strPasswd,strName,strEmail,wSerial) VALUES (?, ?, ?, ?, ?)', $_POST['Username'], $_POST['Password'], $_POST['strName'], $_POST['Email'], $_POST['wSerial'])
      and $messages[] = 'Your Account has now been created.'
  );
  return implode('<br />', $messages); 
}
?>
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Soma Register Page</title>
</head>
<body>
<b> 
  <?php isset($_POST['AddAccount']) and print response(); ?>
</b>
<table width="568" border="0" align="center" bgcolor="#000">
  <tr>
    <td width="115" height="100">&nbsp;</td>
    <td width="347">&nbsp;</td>
    <td width="92">&nbsp;</td>
  </tr>
  <tr>
    <td height="233">&nbsp;</td>
    <td valign="top" class="mytext"> <u><font color="#FFFFFF"></font></u><br>
      <form action="register.php" method="post">
        <table width="347" border="0">
          <tr>
            <td width="175" height="21"><font color="#FFFFFF">Username: </font></td>
            <td width="120" valign="top"><input type="text" name="Username" height="21" maxlength="12"

style="width: 120px;"> </td>
            <td width="38">&nbsp;</td>
          </tr>
          <tr>
            <td height="21"><font color="#FFFFFF">Password: </font></td>
            <td valign="top"><input type="password" name="Password" height="21" maxlength="12"

style="width: 120px;"></td>
            <td>&nbsp;</td>
          </tr>
          <tr>
            <td height="21" valign="top"><font color="#FFFFFF">Security Code:
              (5 Digits)</font></td>
            <td valign="top"><input type="password" name="wSerial" height="21" maxlength="5"

style="width: 120px;"></td>
            <td></td>
          </tr>
          <tr>
            <td height="21" valign="top"><font color="#FFFFFF">Full Name: </font></td>
            <td valign="top"><input type="text" name="strName" height="21" maxlength="20"

style="width: 120px;"></td>
            <td></td>
          </tr>
          <tr>
            <td height="21" valign="top"><font color="#FFFFFF">Email Address:
              </font></td>
            <td valign="top"><input type="text" name="Email" height="21" maxlength="50"

style="width: 120px;"></td>
            <td></td>
          </tr>
          <tr>
            <td height="21"></td>
            <td></td>
            <td></td>
          <tr>
            <td height="21"></td>
            <td valign="top"><input type= "submit" value="Submit" name="AddAccount"></td>
            <td>&nbsp;</td>
          </tr>
        </table>
      </form></td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td height="117">&nbsp;</td>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
  </tr>
</table>
</body>