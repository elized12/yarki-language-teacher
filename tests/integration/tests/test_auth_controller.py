import requests
import json
import pytest
from conftest import APP_HOST, APP_PORT, conn


@pytest.mark.endpoint("/sign-up")
def test_register_user():
    email = "test@mail.ru"
    nickname = "test"
    password = "Kirill200415!"

    request_data = json.dumps(
        {"email": email, "nickname": nickname, "password": password}
    )

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/sign-up",
        request_data,
        headers={"Content-Type": "application/json"},
    )
    assert (
        response.status_code == 201
    ), f"Response body: {response.text.encode().decode('unicode-escape')}"

    response_data = response.json()
    assert response_data["status"] == True

    with conn.cursor() as curr:
        curr.execute('SELECT email, nickname, hashed_password, id FROM "user"')

        user = curr.fetchone()

        assert user is not None

        db_email, db_nickname, db_password, db_id = user

        assert email == db_email

        assert nickname == db_nickname

        assert password == db_password


@pytest.mark.endpoint("/sign-up")
def test_register_user_notValidateEmail():
    email = "test"
    nickname = "test"
    password = "Kirill200415!"

    request_data = json.dumps(
        {"email": email, "nickname": nickname, "password": password}
    )

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/sign-up",
        request_data,
        headers={"Content-Type": "application/json"},
    )
    assert (
        response.status_code == 400
    ), f"Response body: {response.text.encode().decode('unicode-escape')}"

    response_data = response.json()
    assert response_data["status"] == False


@pytest.mark.endpoint("/sign-up")
def test_register_user_notValidateNickname():
    email = "test@mail.ru"
    nickname = "тест"
    password = "Kirill200415!"

    request_data = json.dumps(
        {"email": email, "nickname": nickname, "password": password}
    )

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/sign-up",
        request_data,
        headers={"Content-Type": "application/json"},
    )
    assert (
        response.status_code == 400
    ), f"Response body: {response.text.encode().decode('unicode-escape')}"

    response_data = response.json()
    assert response_data["status"] == False


@pytest.mark.endpoint("/sign-up")
def test_register_user_notValidatePassword():
    email = "test@mail.ru"
    nickname = "test"
    password = "Kirill200415"

    request_data = json.dumps(
        {"email": email, "nickname": nickname, "password": password}
    )

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/sign-up",
        request_data,
        headers={"Content-Type": "application/json"},
    )
    assert (
        response.status_code == 400
    ), f"Response body: {response.text.encode().decode('unicode-escape')}"

    response_data = response.json()
    assert response_data["status"] == False


@pytest.mark.endpoint("/sign-up")
def test_register_user_userWithEmailAlreadyExist():
    first_email = "test@mail.ru"
    first_nickname = "test"
    first_password = "Kirill200415!"

    request_data = json.dumps(
        {"email": first_email, "nickname": first_nickname, "password": first_password}
    )

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/sign-up",
        request_data,
        headers={"Content-Type": "application/json"},
    )
    assert (
        response.status_code == 201
    ), f"Response body: {response.text.encode().decode('unicode-escape')}"

    response_data = response.json()
    assert response_data["status"] == True

    second_email = "test@mail.ru"
    second_nickname = "test123"
    second_password = "Kirill200415!"

    request_data = json.dumps(
        {
            "email": second_email,
            "nickname": second_nickname,
            "password": second_password,
        }
    )

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/sign-up",
        request_data,
        headers={"Content-Type": "application/json"},
    )

    assert response.status_code == 409

    assert response.json()["status"] == False


@pytest.mark.endpoint("/sign-in")
def test_login_user():
    email = "test@mail.ru"
    password = "Kirill200415!"
    nickname = "test"

    request_data = json.dumps(
        {"email": email, "nickname": nickname, "password": password}
    )

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/sign-up",
        request_data,
        headers={"Content-Type": "application/json"},
    )
    assert (
        response.status_code == 201
    ), f"Response body: {response.text.encode().decode('unicode-escape')}"

    response_data = response.json()
    assert response_data["status"] == True

    request_data = json.dumps({"email": email, "password": password})

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/sign-in",
        request_data,
        headers={"Content-Type": "application/json"},
    )

    assert (
        response.status_code == 200
    ), f"Response body: {response.text.encode().decode('unicode-escape')}"

    assert response.json()["status"] == True

    assert len(response.json()["access_token"]) != 0
    assert len(response.json()["refresh_token"]) != 0
    assert response.json()["user"]["email"] == email
    assert response.json()["user"]["nickname"] == nickname
    assert len(response.json()["user"]["id"]) != 0


@pytest.mark.endpoint("/sign-in")
def test_login_user_notExistUser():
    email = "test@mail.ru"
    password = "Kirill200415!"

    request_data = json.dumps({"email": email, "password": password})

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/sign-in",
        request_data,
        headers={"Content-Type": "application/json"},
    )

    assert response.status_code == 400

    assert response.json()["status"] == False


@pytest.mark.endpoint("/sign-in")
def test_login_user_notValidateEmail():
    email = "test"
    password = "Kirill200415!"

    request_data = json.dumps({"email": email, "password": password})

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/sign-in",
        request_data,
        headers={"Content-Type": "application/json"},
    )

    assert (
        response.status_code == 400
    ), f"Response body: {response.text.encode().decode('unicode-escape')}"

    assert response.json()["status"] == False


@pytest.mark.endpoint("/sign-in")
def test_login_user_notValidatePassword():
    email = "test@mail.ru"
    password = "Kirill200415"

    request_data = json.dumps({"email": email, "password": password})

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/sign-in",
        request_data,
        headers={"Content-Type": "application/json"},
    )

    assert (
        response.status_code == 400
    ), f"Response body: {response.text.encode().decode('unicode-escape')}"

    assert response.json()["status"] == False


@pytest.mark.endpoint("/refresh")
def test_refresh():
    email = "test@mail.ru"
    password = "Kirill200415!"
    nickname = "test"

    request_data = json.dumps(
        {"email": email, "password": password, "nickname": nickname}
    )

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/sign-up",
        request_data,
        headers={"Content-Type": "application/json"},
    )

    assert (
        response.status_code == 201
    ), f"Response body: {response.text.encode().decode('unicode-escape')}"

    assert response.json()["status"] == True

    request_data = json.dumps({"email": email, "password": password})

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/sign-in",
        request_data,
        headers={"Content-Type": "application/json"},
    )

    assert (
        response.status_code == 200
    ), f"Response body: {response.text.encode().decode('unicode-escape')}"
    assert response.json()["status"] == True

    assert len(response.json()["access_token"]) != 0
    assert len(response.json()["refresh_token"]) != 0

    request_data = json.dumps({"refresh_token": response.json()["refresh_token"]})

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/refresh",
        request_data,
        headers={"Content-Type": "application/json"},
    )

    assert (
        response.status_code == 200
    ), f"Response body: {response.text.encode().decode('unicode-escape')}, request_data : {request_data}"

    assert len(response.json()["access_token"]) != 0


@pytest.mark.endpoint("/refresh")
def test_refresh_notValidate():
    email = "test@mail.ru"
    password = "Kirill200415!"
    nickname = "test"

    request_data = json.dumps(
        {"email": email, "password": password, "nickname": nickname}
    )

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/sign-up",
        request_data,
        headers={"Content-Type": "application/json"},
    )

    assert (
        response.status_code == 201
    ), f"Response body: {response.text.encode().decode('unicode-escape')}"

    assert response.json()["status"] == True

    request_data = json.dumps({"email": email, "password": password})

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/sign-in",
        request_data,
        headers={"Content-Type": "application/json"},
    )

    assert (
        response.status_code == 200
    ), f"Response body: {response.text.encode().decode('unicode-escape')}"
    assert response.json()["status"] == True

    assert len(response.json()["access_token"]) != 0
    assert len(response.json()["refresh_token"]) != 0

    request_data = json.dumps(
        {"refresh_token": response.json()["refresh_token"] + "test"}
    )

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/refresh",
        request_data,
        headers={"Content-Type": "application/json"},
    )

    assert (
        response.status_code == 400
    ), f"Response body: {response.text.encode().decode('unicode-escape')}, request_data : {request_data}"


@pytest.mark.endpoint("/refresh")
def test_refresh_tokenRevoked():
    email = "test@mail.ru"
    password = "Kirill200415!"
    nickname = "test"

    request_data = json.dumps(
        {"email": email, "password": password, "nickname": nickname}
    )

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/sign-up",
        request_data,
        headers={"Content-Type": "application/json"},
    )

    assert (
        response.status_code == 201
    ), f"Response body: {response.text.encode().decode('unicode-escape')}"

    assert response.json()["status"] == True

    request_data = json.dumps({"email": email, "password": password})

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/sign-in",
        request_data,
        headers={"Content-Type": "application/json"},
    )

    assert (
        response.status_code == 200
    ), f"Response body: {response.text.encode().decode('unicode-escape')}"
    assert response.json()["status"] == True

    assert len(response.json()["access_token"]) != 0
    assert len(response.json()["refresh_token"]) != 0

    with conn.cursor() as curr:
        curr.execute("UPDATE token SET revoked_at = CURRENT_TIMESTAMP")
        conn.commit()

        request_data = json.dumps({"refresh_token": response.json()["refresh_token"]})

        response = requests.post(
            f"http://{APP_HOST}:{APP_PORT}/refresh",
            request_data,
            headers={"Content-Type": "application/json"},
        )

        assert (
            response.status_code == 400
        ), f"Response body: {response.text.encode().decode('unicode-escape')}, request_data : {request_data}"


@pytest.mark.endpoint("/refresh")
def test_refresh_expiredToken():
    email = "test@mail.ru"
    password = "Kirill200415!"
    nickname = "test"

    request_data = json.dumps(
        {"email": email, "password": password, "nickname": nickname}
    )

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/sign-up",
        request_data,
        headers={"Content-Type": "application/json"},
    )

    assert (
        response.status_code == 201
    ), f"Response body: {response.text.encode().decode('unicode-escape')}"

    assert response.json()["status"] == True

    request_data = json.dumps({"email": email, "password": password})

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/sign-in",
        request_data,
        headers={"Content-Type": "application/json"},
    )

    assert (
        response.status_code == 200
    ), f"Response body: {response.text.encode().decode('unicode-escape')}"
    assert response.json()["status"] == True

    assert len(response.json()["access_token"]) != 0
    assert len(response.json()["refresh_token"]) != 0

    with conn.cursor() as curr:
        curr.execute("UPDATE token SET expired_at = CURRENT_TIMESTAMP")
        conn.commit()

        request_data = json.dumps({"refresh_token": response.json()["refresh_token"]})

        response = requests.post(
            f"http://{APP_HOST}:{APP_PORT}/refresh",
            request_data,
            headers={"Content-Type": "application/json"},
        )

        assert (
            response.status_code == 400
        ), f"Response body: {response.text.encode().decode('unicode-escape')}, request_data : {request_data}"
