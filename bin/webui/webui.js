let url = 'https://wifievent.io/api';

const webReq = axios.create({
  baseURL: url,
  withCredentials: true,
  timeout: 1000,
})

window.onload = async function () {
  const sessRes = await webReq({
    url: '/user/session',
  });
  if (sessRes.status !== 200)
    return;

  exchangeUI();

  const pageRes = await webReq({
    url: '/cp/page',
  })
  if (pageRes.status !== 200)
    return;
  
  $('#browsers').empty();
  for (const page of pageRes.data) {
    const option = $(`<option value='${page.pid}'>${page.name}</option>`);
    $('#browsers').append(option);
  }
};

document.querySelector('#button').addEventListener('click', async function () {
  const uid = document.querySelector('#uid').value;
  const pw = document.querySelector('#pw').value;
  if (!uid || !pw) {
    alert('fill in the blank');
    return;
  }

  const body = { uid, pw };

  const loginRes = await webReq({
    method: 'POST',
    url: '/user/login',
    data: body,
  }).catch(err => {
    alert(err.response.statusText);
    return;
  });
  if (loginRes.status !== 200)
    return;

  alert('login success');
  location.reload();
});

function exchangeUI(pageList) {
  $('#idbox').remove();
  $('#pwbox').remove();
  $('#buttonbox').remove();
  $('#main').append(`<div class='input-wrap' id='idbox'> \
                      <select class='input-wrap' id='browsers' name='browsers'>\
                      </select>\
                    </div> \
                    <div class='blue-button-wrap button-wrap' id='buttonbox'> \
                      <button type='button' onclick='cpStatus()' id='button'>Start</button> \
                    </div>`);
}

async function cpStatus() {
  let status = $('button').text();
  const pid = document.querySelector('#browsers').value;
  await axios.post(`/${status}`, `${pid}`).catch(err => {
    alert(`${status} Failed`);
    return;
  });

  let buttonColor;
  if (status == 'Start') {
    buttonColor = 'red';
    status = 'Stop';
  } else {
    buttonColor = 'blue';
    status = 'Start';
  }
  $('#buttonbox').attr('class',`${buttonColor}-button-wrap button-wrap`);
  $('#button').text(status);
}
