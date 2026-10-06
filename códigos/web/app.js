// ===== Mensagens =====

const painelMensagens = document.getElementById("mensagens");
const LIMITE_MENSAGENS = 6;

// acrescenta mensagens no painel (as que vêm do cout do C++ e as da própria tela)
function mostrarMensagens(lista) {
  for (const texto of lista) {
    const linha = document.createElement("div");
    linha.textContent = "> " + texto;
    painelMensagens.appendChild(linha);
  }

  // mantém só as últimas mensagens
  while (painelMensagens.children.length > LIMITE_MENSAGENS) {
    painelMensagens.removeChild(painelMensagens.firstChild);
  }
}

document.getElementById("limpar-mensagens").addEventListener("click", function () {
  painelMensagens.innerHTML = "";
});


// ===== Comunicação com o C++ =====

// chama um caminho da API; sem "dados" faz um GET, com "dados" faz um POST
// os dados vão como formulário (nome=ana&senha=123), que o httplib já sabe ler no C++
// as mensagens que vierem na resposta já aparecem no painel
async function api(caminho, dados) {
  const opcoes = dados === undefined
    ? {}
    : { method: "POST", body: new URLSearchParams(dados) };

  let resposta;
  try {
    resposta = await fetch(caminho, opcoes);
  } catch (erro) {
    mostrarMensagens(["Não foi possível falar com o servidor. Ele está rodando?"]);
    return { ok: false, mensagens: [] };
  }

  // o servidor respondeu, mas não conhece esse caminho (ex.: servidor antigo ainda rodando)
  if (resposta.status === 404) {
    const texto = "O servidor não conhece " + caminho.split("?")[0] + ". Ele foi recompilado e reiniciado?";
    mostrarMensagens([texto]);
    return { ok: false, mensagens: [texto] };
  }

  try {
    const json = await resposta.json();

    if (json.mensagens) {
      mostrarMensagens(json.mensagens);
    }
    return json;
  } catch (erro) {
    const texto = "Resposta inválida do servidor (código " + resposta.status + ").";
    mostrarMensagens([texto]);
    return { ok: false, mensagens: [texto] };
  }
}


// ===== Navegação entre as telas =====

const TELAS = ["catalogo", "conta", "admin"];

// abre a tela que está no endereço (#catalogo, #conta ou #admin)
function abrirTela() {
  const pedida = location.hash.slice(1);
  const atual = TELAS.includes(pedida) ? pedida : "catalogo";

  document.querySelectorAll("main section[data-tela]").forEach(function (secao) {
    secao.hidden = secao.dataset.tela !== atual;
  });

  document.querySelectorAll("nav a[data-tela]").forEach(function (link) {
    if (link.dataset.tela === atual) {
      link.setAttribute("aria-current", "page");
    } else {
      link.removeAttribute("aria-current");
    }
  });

  // a administração sempre mostra os dados mais recentes ao ser aberta
  if (atual === "admin" && typeof carregarAdmin === "function") {
    carregarAdmin();
  }
}

window.addEventListener("hashchange", abrirTela);
abrirTela();


// ===== Ajudantes para montar a tela =====

// cria um elemento com classes e texto (textContent evita que um título vire código HTML)
function criar(tag, classes, texto) {
  const elemento = document.createElement(tag);
  if (classes) {
    elemento.className = classes;
  }
  if (texto !== undefined) {
    elemento.textContent = texto;
  }
  return elemento;
}

const formatoReais = new Intl.NumberFormat("pt-BR", { style: "currency", currency: "BRL" });

function formatarTamanho(gigabytes) {
  return gigabytes.toLocaleString("pt-BR") + " GB";
}

function formatarPreco(jogo) {
  return jogo.pago ? formatoReais.format(jogo.preco) : "Grátis";
}

// etiqueta "Pago" (âmbar) ou "Gratuito" (cinza)
function etiquetaTipo(pago) {
  const classes = pago
    ? "inline-block px-2.5 py-0.5 rounded-full text-[13px] font-medium bg-[#2A2113] text-aviso border border-[#4A3818]"
    : "inline-block px-2.5 py-0.5 rounded-full text-[13px] font-medium bg-elevado text-[#B4BDC9] border border-[#2F3846]";
  return criar("span", classes, pago ? "Pago" : "Gratuito");
}


// ===== Catálogo =====

// o que está escolhido na tela; a busca, o filtro e a ordenação são feitos pelo C++
const estadoCatalogo = {
  busca: "",
  tipo: "",
  ordem: "id",
  direcao: "asc",
  jogos: [],
  selecionado: null
};

const DESCRICAO_ORDEM = {
  id: { asc: "id crescente", desc: "id decrescente" },
  titulo: { asc: "título de A a Z", desc: "título de Z a A" },
  tamanho: { asc: "menor ao maior tamanho", desc: "maior ao menor tamanho" },
  preco: { asc: "menor ao maior preço", desc: "maior ao menor preço" }
};

// pede a lista ao C++ com as opções atuais
async function carregarCatalogo() {
  const parametros = new URLSearchParams({
    busca: estadoCatalogo.busca,
    tipo: estadoCatalogo.tipo,
    ordem: estadoCatalogo.ordem,
    direcao: estadoCatalogo.direcao
  });

  const resposta = await api("/api/jogos?" + parametros.toString());
  if (resposta.ok) {
    estadoCatalogo.jogos = resposta.jogos;
    desenharCatalogo();
  }
}

function desenharCatalogo() {
  const linhas = document.getElementById("catalogo-linhas");
  linhas.innerHTML = "";

  for (const jogo of estadoCatalogo.jogos) {
    const selecionado = jogo.id === estadoCatalogo.selecionado;
    const linha = criar("tr", selecionado ? "bg-selecionado shadow-[inset_3px_0_0_#2F6FE0]" : "");

    linha.appendChild(criar("td", "px-4 h-12 border-b border-linha font-mono text-suave", String(jogo.id)));

    const celulaTitulo = criar("td", "px-2 border-b border-linha");
    const botaoTitulo = criar("button", "min-h-11 w-full px-2 text-left font-medium hover:text-link", jogo.titulo);
    botaoTitulo.type = "button";
    botaoTitulo.addEventListener("click", function () {
      estadoCatalogo.selecionado = jogo.id;
      desenharCatalogo();
    });
    celulaTitulo.appendChild(botaoTitulo);
    linha.appendChild(celulaTitulo);

    linha.appendChild(criar("td", "px-4 border-b border-linha text-right font-mono text-[#C9D1DB]", formatarTamanho(jogo.tamanho)));

    const celulaTipo = criar("td", "px-4 border-b border-linha");
    celulaTipo.appendChild(etiquetaTipo(jogo.pago));
    linha.appendChild(celulaTipo);

    linha.appendChild(criar("td", "px-4 border-b border-linha text-right font-mono", formatarPreco(jogo)));
    linhas.appendChild(linha);
  }

  document.getElementById("catalogo-vazio").hidden = estadoCatalogo.jogos.length > 0;

  const quantidade = estadoCatalogo.jogos.length;
  document.getElementById("catalogo-resumo").textContent =
    (quantidade === 1 ? "1 jogo" : quantidade + " jogos") + " · ordenado por " +
    DESCRICAO_ORDEM[estadoCatalogo.ordem][estadoCatalogo.direcao];

  // setas e aria-sort dos cabeçalhos
  document.querySelectorAll("#tela-catalogo th[data-coluna]").forEach(function (cabecalho) {
    const ativa = cabecalho.dataset.coluna === estadoCatalogo.ordem;
    const crescente = estadoCatalogo.direcao === "asc";
    cabecalho.setAttribute("aria-sort", ativa ? (crescente ? "ascending" : "descending") : "none");
    cabecalho.querySelector(".seta").textContent = ativa ? (crescente ? "↑" : "↓") : "↕";
    cabecalho.querySelector("button").classList.toggle("text-texto", ativa);
  });

  desenharDetalhes();
}

// painel da direita com o jogo selecionado
function desenharDetalhes() {
  const painel = document.getElementById("catalogo-detalhes");
  painel.innerHTML = "";

  const jogo = estadoCatalogo.jogos.find(function (j) { return j.id === estadoCatalogo.selecionado; });

  if (!jogo) {
    painel.appendChild(criar("p", "text-suave", "Selecione um jogo na lista para ver os detalhes."));
    return;
  }

  painel.appendChild(criar("div", "text-[13px] uppercase tracking-wider text-suave", "Jogo selecionado"));
  painel.appendChild(criar("div", "text-[22px] font-semibold leading-tight", jogo.titulo));

  const etiqueta = criar("div");
  etiqueta.appendChild(etiquetaTipo(jogo.pago));
  painel.appendChild(etiqueta);

  const lista = criar("dl", "grid grid-cols-2 gap-x-4 gap-y-3");
  const itens = [["Id", String(jogo.id)], ["Tamanho", formatarTamanho(jogo.tamanho)], ["Preço", formatarPreco(jogo)]];
  for (const item of itens) {
    lista.appendChild(criar("dt", "text-suave", item[0]));
    lista.appendChild(criar("dd", "text-right font-mono", item[1]));
  }
  painel.appendChild(lista);

  desenharCompra(painel, jogo);
}

// parte de baixo do painel: o botão de compra muda conforme a situação
function desenharCompra(painel, jogo) {
  // ninguém logado: botão desativado com o aviso
  if (contaAtual === null) {
    const botao = criar("button", "min-h-11 rounded-md bg-borda text-suave font-semibold cursor-not-allowed",
      jogo.pago ? "Comprar" : "Adicionar à biblioteca");
    botao.type = "button";
    botao.disabled = true;
    painel.appendChild(botao);
    painel.appendChild(criar("p", "text-[13px] text-suave", "Entre na conta para adicionar jogos à sua biblioteca."));
    return;
  }

  // a conta já tem o jogo
  const jaPossui = contaAtual.biblioteca.some(function (item) { return item.id === jogo.id; });
  if (jaPossui) {
    const botao = criar("button", "min-h-11 rounded-md bg-borda text-suave font-semibold cursor-not-allowed", "Já está na sua biblioteca");
    botao.type = "button";
    botao.disabled = true;
    painel.appendChild(botao);
    return;
  }

  const status = criar("div", "", "");
  status.hidden = true;

  // jogo gratuito: entra direto, sem escolher pagamento
  if (!jogo.pago) {
    const botao = criar("button", "min-h-11 rounded-md bg-destaque text-white font-semibold hover:brightness-110", "Adicionar à biblioteca");
    botao.type = "button";
    botao.addEventListener("click", function () { comprarJogo(jogo, { id: jogo.id }, status); });
    painel.appendChild(botao);
    painel.appendChild(status);
    return;
  }

  // jogo pago: escolhe saldo ou um dos cartões cadastrados
  const grupo = criar("fieldset", "flex flex-col gap-2 border-t border-linha pt-3");
  grupo.appendChild(criar("legend", "text-[#C9D1DB] font-medium mb-1", "Pagar com"));

  const opcoes = [{ valor: "saldo", texto: "Saldo (" + formatoReais.format(contaAtual.saldo) + ")" }];
  for (const cartao of contaAtual.cartoes) {
    opcoes.push({
      valor: "cartao-" + cartao.indice,
      texto: "Cartão final " + cartao.final + " (disponível " + formatoReais.format(cartao.limite - cartao.gastos) + ")"
    });
  }

  opcoes.forEach(function (opcao, posicao) {
    const rotulo = criar("label", "flex items-center gap-2.5 min-h-10 cursor-pointer");
    const radio = criar("input", "accent-[#2F6FE0] w-4 h-4");
    radio.type = "radio";
    radio.name = "forma-pagamento";
    radio.value = opcao.valor;
    radio.checked = posicao === 0;
    rotulo.appendChild(radio);
    rotulo.appendChild(criar("span", "", opcao.texto));
    grupo.appendChild(rotulo);
  });
  painel.appendChild(grupo);

  if (contaAtual.cartoes.length === 0) {
    painel.appendChild(criar("p", "text-[13px] text-suave", "Para pagar com cartão, cadastre um na Carteira."));
  }

  const botao = criar("button", "min-h-11 rounded-md bg-destaque text-white font-semibold hover:brightness-110",
    "Comprar por " + formatoReais.format(jogo.preco));
  botao.type = "button";
  botao.addEventListener("click", function () {
    const escolhida = painel.querySelector("input[name=forma-pagamento]:checked").value;
    const dados = { id: jogo.id, forma: "saldo" };
    if (escolhida.startsWith("cartao-")) {
      dados.forma = "cartao";
      dados.cartao = escolhida.slice(7);
    }
    comprarJogo(jogo, dados, status);
  });
  painel.appendChild(botao);
  painel.appendChild(status);
}

// pede a compra ao C++; se der certo, atualiza a conta e o painel
async function comprarJogo(jogo, dados, status) {
  const resposta = await api("/api/comprar", dados);
  if (resposta.ok) {
    atualizarSessao(resposta.conta);
  } else {
    mostrarStatus(status, false, ultimaMensagem(resposta, "Não foi possível comprar."));
  }
}

// busca: espera a pessoa parar de digitar um pouco antes de pedir ao C++
let esperaBusca = null;
document.getElementById("catalogo-busca").addEventListener("input", function (evento) {
  clearTimeout(esperaBusca);
  esperaBusca = setTimeout(function () {
    estadoCatalogo.busca = evento.target.value.trim();
    carregarCatalogo();
  }, 250);
});

// filtro Todos / Gratuitos / Pagos
document.querySelectorAll("#tela-catalogo button[data-tipo]").forEach(function (botao) {
  botao.addEventListener("click", function () {
    estadoCatalogo.tipo = botao.dataset.tipo;
    document.querySelectorAll("#tela-catalogo button[data-tipo]").forEach(function (outro) {
      outro.setAttribute("aria-pressed", outro === botao ? "true" : "false");
    });
    carregarCatalogo();
  });
});

// ordenação: clicar na mesma coluna inverte; em outra, começa em crescente
document.querySelectorAll("#tela-catalogo button[data-ordem]").forEach(function (botao) {
  botao.addEventListener("click", function () {
    if (estadoCatalogo.ordem === botao.dataset.ordem) {
      estadoCatalogo.direcao = estadoCatalogo.direcao === "asc" ? "desc" : "asc";
    } else {
      estadoCatalogo.ordem = botao.dataset.ordem;
      estadoCatalogo.direcao = "asc";
    }
    carregarCatalogo();
  });
});


// ===== Conta: entrar, criar conta e sair =====

let contaAtual = null; // conta logada (nome, saldo, biblioteca, cartões), ou null
let abaAcesso = "entrar";

// mostra no cabeçalho e na tela Minha conta se há alguém logado
function atualizarSessao(conta) {
  contaAtual = conta;
  const logado = conta !== null;

  document.getElementById("cabecalho-entrar").hidden = logado;
  document.getElementById("cabecalho-conectado").hidden = !logado;
  document.getElementById("conta-area-acesso").hidden = logado;
  document.getElementById("conta-painel").hidden = !logado;

  if (logado) {
    document.getElementById("cabecalho-nome").textContent = conta.nome;
    desenharConta();
  }

  // o painel de detalhes do catálogo muda (botão de compra) conforme há alguém logado
  desenharDetalhes();
}

// mostra uma mensagem numa caixa (âmbar para erro, azul para sucesso)
function mostrarStatus(caixa, sucesso, texto) {
  caixa.hidden = false;
  caixa.textContent = texto;
  caixa.className = sucesso
    ? "px-3.5 py-3 rounded-md font-medium bg-[#13233B] text-[#A9CBFF] border border-[#23406A]"
    : "px-3.5 py-3 rounded-md font-medium bg-[#2A2113] text-aviso border border-[#4A3818]";
}

// última mensagem que o C++ mandou (ou um texto padrão)
function ultimaMensagem(resposta, padrao) {
  const lista = resposta.mensagens || [];
  return lista.length > 0 ? lista[lista.length - 1] : padrao;
}

// mensagem logo abaixo do formulário de acesso
function mostrarStatusAcesso(sucesso, texto) {
  mostrarStatus(document.getElementById("acesso-status"), sucesso, texto);
}

// troca entre as abas Entrar e Criar conta
function escolherAba(aba) {
  abaAcesso = aba;
  const criando = aba === "criar";

  document.getElementById("aba-entrar").setAttribute("aria-selected", criando ? "false" : "true");
  document.getElementById("aba-criar").setAttribute("aria-selected", criando ? "true" : "false");
  document.getElementById("acesso-titulo").textContent = criando ? "Criar conta" : "Entrar na conta";
  document.getElementById("acesso-subtitulo").textContent = criando ? "Escolha um nome e uma senha." : "Use o nome e a senha da sua conta.";
  document.getElementById("acesso-botao").textContent = criando ? "Criar conta" : "Entrar";
  document.getElementById("acesso-confirma-campo").hidden = !criando;
  document.getElementById("acesso-senha").autocomplete = criando ? "new-password" : "current-password";
  document.getElementById("acesso-confirma").value = "";
  document.getElementById("acesso-status").hidden = true;
}

document.getElementById("aba-entrar").addEventListener("click", function () { escolherAba("entrar"); });
document.getElementById("aba-criar").addEventListener("click", function () { escolherAba("criar"); });

// botão Entrar / Criar conta (também funciona apertando Enter)
document.getElementById("acesso-form").addEventListener("submit", async function (evento) {
  evento.preventDefault();

  const nome = document.getElementById("acesso-nome").value.trim();
  const senha = document.getElementById("acesso-senha").value;
  const confirma = document.getElementById("acesso-confirma").value;

  if (nome === "" || senha === "") {
    mostrarStatusAcesso(false, "Preencha o nome e a senha.");
    return;
  }

  if (abaAcesso === "entrar") {
    const resposta = await api("/api/entrar", { nome: nome, senha: senha });
    document.getElementById("acesso-senha").value = "";

    if (resposta.ok) {
      document.getElementById("acesso-status").hidden = true;
      atualizarSessao(resposta.conta);
    } else {
      mostrarStatusAcesso(false, resposta.mensagens[resposta.mensagens.length - 1] || "Não foi possível entrar.");
    }
    return;
  }

  // criar conta: a confirmação da senha é conferida só aqui na tela
  if (senha !== confirma) {
    mostrarStatusAcesso(false, "As senhas não conferem.");
    return;
  }

  const resposta = await api("/api/contas", { nome: nome, senha: senha });
  const ultima = resposta.mensagens[resposta.mensagens.length - 1] || "";

  if (resposta.ok) {
    escolherAba("entrar");
    document.getElementById("acesso-senha").value = "";
    mostrarStatusAcesso(true, ultima);
  } else {
    mostrarStatusAcesso(false, ultima || "Não foi possível criar a conta.");
  }
});

// sair da conta (pelo cabeçalho ou pelo cartão)
async function sairDaConta() {
  const resposta = await api("/api/sair", {});
  if (resposta.ok) {
    atualizarSessao(null);
    document.getElementById("acesso-nome").value = "";
    escolherAba("entrar");
  }
}

document.getElementById("cabecalho-sair").addEventListener("click", sairDaConta);


// ===== Minha conta: biblioteca =====

let jogoSelecionadoConta = null;
let confirmandoReembolso = false;

function formatarData(segundos) {
  return segundos > 0 ? new Date(segundos * 1000).toLocaleDateString("pt-BR") : "—";
}

// como o jogo foi pago: 0 = gratuito, 1 = saldo, 2 = cartão
function formatarPagamento(item) {
  if (item.forma === 2) {
    return item.cartao_final ? "Cartão final " + item.cartao_final : "Cartão";
  }
  return item.forma === 1 ? "Saldo" : "Gratuito";
}

function etiquetaSituacao(instalado) {
  const classes = instalado
    ? "inline-block px-2.5 py-0.5 rounded-full text-[13px] font-medium bg-[#13233B] text-[#A9CBFF] border border-[#23406A]"
    : "inline-block px-2.5 py-0.5 rounded-full text-[13px] font-medium bg-elevado text-[#B4BDC9] border border-[#2F3846]";
  return criar("span", classes, instalado ? "Instalado" : "Não instalado");
}

function desenharConta() {
  const conta = contaAtual;
  const horas = conta.biblioteca.reduce(function (total, item) { return total + item.horas; }, 0);

  document.getElementById("conta-saldo").textContent = formatoReais.format(conta.saldo);
  document.getElementById("conta-total-jogos").textContent = String(conta.biblioteca.length);
  document.getElementById("conta-total-horas").textContent = horas + " h";

  const linhas = document.getElementById("biblioteca-linhas");
  linhas.innerHTML = "";

  for (const item of conta.biblioteca) {
    const selecionado = item.id === jogoSelecionadoConta;
    const linha = criar("tr", selecionado ? "bg-selecionado shadow-[inset_3px_0_0_#2F6FE0]" : "");

    const celulaTitulo = criar("td", "px-2 border-b border-linha");
    const botao = criar("button", "min-h-11 w-full px-2 text-left font-medium hover:text-link", item.titulo);
    botao.type = "button";
    botao.addEventListener("click", function () {
      jogoSelecionadoConta = item.id;
      confirmandoReembolso = false;
      desenharConta();
    });
    celulaTitulo.appendChild(botao);
    linha.appendChild(celulaTitulo);

    const celulaSituacao = criar("td", "px-4 border-b border-linha");
    celulaSituacao.appendChild(etiquetaSituacao(item.instalado));
    linha.appendChild(celulaSituacao);

    linha.appendChild(criar("td", "px-4 border-b border-linha text-right font-mono", String(item.horas)));
    linha.appendChild(criar("td", "px-4 border-b border-linha font-mono text-[#C9D1DB]", formatarData(item.data_compra)));
    linha.appendChild(criar("td", "px-4 border-b border-linha text-[#C9D1DB]", formatarPagamento(item)));
    linhas.appendChild(linha);
  }

  document.getElementById("biblioteca-vazia").hidden = conta.biblioteca.length > 0;
  desenharAcoesConta();
  desenharCarteira();
}

// painel da direita: instalar, jogar e reembolsar o jogo selecionado
function desenharAcoesConta() {
  const painel = document.getElementById("biblioteca-detalhes");
  painel.innerHTML = "";

  const item = contaAtual.biblioteca.find(function (i) { return i.id === jogoSelecionadoConta; });
  if (!item) {
    painel.appendChild(criar("p", "text-suave", "Selecione um jogo da biblioteca para ver as ações."));
    return;
  }

  const status = criar("div");
  status.setAttribute("role", "status");
  status.hidden = true;

  painel.appendChild(criar("div", "text-[13px] uppercase tracking-wider text-suave", "Jogo selecionado"));
  painel.appendChild(criar("div", "text-[22px] font-semibold leading-tight", item.titulo));

  const lista = criar("dl", "grid grid-cols-2 gap-x-4 gap-y-2.5");
  const dados = [
    ["Situação", item.instalado ? "Instalado" : "Não instalado"],
    ["Horas jogadas", String(item.horas)],
    ["Valor pago", item.valor_pago > 0 ? formatoReais.format(item.valor_pago) : "Grátis"]
  ];
  for (const dado of dados) {
    lista.appendChild(criar("dt", "text-suave", dado[0]));
    lista.appendChild(criar("dd", "text-right font-mono", dado[1]));
  }
  painel.appendChild(lista);

  // instalar / desinstalar
  const botaoInstalar = criar("button", "min-h-11 rounded-md border border-borda bg-elevado font-medium hover:border-suave",
    item.instalado ? "Desinstalar" : "Instalar");
  botaoInstalar.type = "button";
  botaoInstalar.addEventListener("click", function () {
    acaoConta(item.instalado ? "/api/desinstalar" : "/api/instalar", { id: item.id }, status);
  });
  painel.appendChild(botaoInstalar);

  // jogar: registra horas
  const blocoJogar = criar("div", "flex flex-col gap-2 border-t border-linha pt-3");
  const rotulo = criar("label", "text-[#C9D1DB] font-medium", "Registrar horas jogadas");
  rotulo.htmlFor = "campo-horas";
  const linhaJogar = criar("div", "flex gap-2");
  const campoHoras = criar("input", "flex-1 min-w-0 min-h-11 px-3 rounded-md bg-fundo border border-borda font-mono outline-none focus:border-destaque");
  campoHoras.id = "campo-horas";
  campoHoras.type = "number";
  campoHoras.min = "1";
  campoHoras.value = "1";
  const botaoJogar = criar("button", "min-h-11 px-[18px] rounded-md bg-destaque text-white font-semibold hover:brightness-110", "Jogar");
  botaoJogar.type = "button";
  botaoJogar.addEventListener("click", function () {
    acaoConta("/api/jogar", { id: item.id, horas: campoHoras.value }, status);
  });
  linhaJogar.appendChild(campoHoras);
  linhaJogar.appendChild(botaoJogar);
  blocoJogar.appendChild(rotulo);
  blocoJogar.appendChild(linhaJogar);
  painel.appendChild(blocoJogar);

  // reembolso: só para jogos pagos; as regras são conferidas no C++
  if (item.pago) {
    const blocoReembolso = criar("div", "flex flex-col gap-2.5 border-t border-linha pt-3");
    blocoReembolso.appendChild(criar("p", "text-[13px] text-suave",
      "Reembolso até " + contaAtual.prazo_reembolso_dias + " dias após a compra e com menos de " +
      contaAtual.limite_horas_reembolso + " horas jogadas."));

    if (!confirmandoReembolso) {
      const botao = criar("button", "min-h-11 rounded-md border border-[#4A3818] text-aviso font-medium hover:bg-[#2A2113]", "Reembolsar");
      botao.type = "button";
      botao.addEventListener("click", function () {
        confirmandoReembolso = true;
        desenharAcoesConta();
      });
      blocoReembolso.appendChild(botao);
    } else {
      const destino = item.forma === 2 && item.cartao_final
        ? "voltam ao cartão final " + item.cartao_final
        : "voltam ao seu saldo";
      blocoReembolso.appendChild(criar("p", "text-aviso",
        "Reembolsar " + item.titulo + "? " + formatoReais.format(item.valor_pago) + " " + destino + "."));

      const botoes = criar("div", "grid grid-cols-2 gap-2");
      const cancelar = criar("button", "min-h-11 rounded-md border border-borda hover:border-suave", "Cancelar");
      cancelar.type = "button";
      cancelar.addEventListener("click", function () {
        confirmandoReembolso = false;
        desenharAcoesConta();
      });
      const confirmar = criar("button", "min-h-11 rounded-md bg-[#8A5A12] text-white font-semibold hover:brightness-110", "Confirmar");
      confirmar.type = "button";
      confirmar.addEventListener("click", function () {
        confirmandoReembolso = false;
        acaoConta("/api/reembolsar", { id: item.id }, status);
      });
      botoes.appendChild(cancelar);
      botoes.appendChild(confirmar);
      blocoReembolso.appendChild(botoes);
    }
    painel.appendChild(blocoReembolso);
  }

  painel.appendChild(status);
}

// faz uma ação da conta no C++ e redesenha a tela com a conta atualizada
async function acaoConta(caminho, dados, status) {
  const resposta = await api(caminho, dados);

  if (resposta.conta) {
    contaAtual = resposta.conta;
    desenharConta();
    desenharDetalhes();
  }

  // o painel foi redesenhado: mostra o resultado na caixa nova
  const caixa = document.querySelector("#biblioteca-detalhes > div[role=status]") || status;
  if (caixa && document.getElementById("biblioteca-detalhes").contains(caixa)) {
    mostrarStatus(caixa, resposta.ok, ultimaMensagem(resposta, resposta.ok ? "Feito." : "Não foi possível."));
  }
}


// ===== Minha conta: abas e carteira =====

document.querySelectorAll("button[data-aba-conta]").forEach(function (aba) {
  aba.addEventListener("click", function () {
    const escolhida = aba.dataset.abaConta;
    document.querySelectorAll("button[data-aba-conta]").forEach(function (outra) {
      outra.setAttribute("aria-selected", outra === aba ? "true" : "false");
    });
    document.getElementById("conteudo-biblioteca").hidden = escolhida !== "biblioteca";
    document.getElementById("conteudo-carteira").hidden = escolhida !== "carteira";
  });
});

// lê um valor digitado aceitando vírgula ou ponto
function lerValor(texto) {
  return parseFloat(String(texto).replace(",", "."));
}

function desenharCarteira() {
  const conta = contaAtual;

  document.getElementById("pix-info").textContent =
    "Chave pix da biblioteca · taxa de " + (conta.pix_taxa * 100).toLocaleString("pt-BR") + "%";
  document.getElementById("pix-chave-loja").textContent = conta.pix_chave;
  atualizarPreviaPix();

  const lista = document.getElementById("lista-cartoes");
  lista.innerHTML = "";

  if (conta.cartoes.length === 0) {
    lista.appendChild(criar("p", "text-suave", "Nenhum cartão cadastrado."));
  }

  for (const cartao of conta.cartoes) {
    const caixa = criar("div", "px-4 py-3.5 rounded-lg bg-fundo border border-borda flex flex-col gap-2.5");

    const topo = criar("div", "flex justify-between gap-3");
    topo.appendChild(criar("span", "font-mono font-medium", "•••• " + cartao.final));
    topo.appendChild(criar("span", "font-mono text-suave", "val. " + cartao.validade));
    caixa.appendChild(topo);

    // barra com quanto do limite já foi usado
    const usado = cartao.limite > 0 ? Math.min(100, Math.round((cartao.gastos / cartao.limite) * 100)) : 0;
    const barra = criar("div", "h-1.5 rounded-full bg-linha overflow-hidden");
    barra.setAttribute("aria-hidden", "true");
    const preenchido = criar("div", "h-full bg-destaque");
    preenchido.style.width = usado + "%";
    barra.appendChild(preenchido);
    caixa.appendChild(barra);

    const valores = criar("div", "flex justify-between gap-3 text-sm text-suave");
    valores.appendChild(criar("span", "", "Disponível " + formatoReais.format(cartao.limite - cartao.gastos)));
    valores.appendChild(criar("span", "", "Limite " + formatoReais.format(cartao.limite)));
    caixa.appendChild(valores);

    lista.appendChild(caixa);
  }
}

// prévia do pix: quanto vai de taxa e quanto entra no saldo
function atualizarPreviaPix() {
  const valor = lerValor(document.getElementById("pix-valor").value);
  const previa = document.getElementById("pix-previa");

  if (!(valor > 0) || contaAtual === null) {
    previa.textContent = "Digite um valor para ver quanto entra no saldo.";
    return;
  }
  const taxa = valor * contaAtual.pix_taxa;
  previa.textContent = "Taxa de " + formatoReais.format(taxa) + ". Entram no saldo " + formatoReais.format(valor - taxa) + ".";
}

document.getElementById("pix-valor").addEventListener("input", atualizarPreviaPix);

// envia um formulário da carteira; se der certo, limpa os campos e atualiza a conta
async function enviarCarteira(formulario, caminho, dados) {
  const status = formulario.querySelector("div[role=status]");
  const resposta = await api(caminho, dados);

  if (resposta.conta) {
    contaAtual = resposta.conta;
    desenharConta();
    desenharDetalhes();
  }
  if (resposta.ok) {
    formulario.reset();
    atualizarPreviaPix();
  }
  mostrarStatus(status, resposta.ok, ultimaMensagem(resposta, resposta.ok ? "Feito." : "Não foi possível."));
}

document.getElementById("form-pix").addEventListener("submit", function (evento) {
  evento.preventDefault();
  const valor = document.getElementById("pix-valor").value.trim();
  const chave = document.getElementById("pix-chave").value.trim();
  if (valor === "" || chave === "") {
    mostrarStatus(this.querySelector("div[role=status]"), false, "Preencha o valor e a chave pix.");
    return;
  }
  enviarCarteira(this, "/api/pix", { valor: valor, chave: chave });
});

document.getElementById("form-gift").addEventListener("submit", function (evento) {
  evento.preventDefault();
  const codigo = document.getElementById("gift-codigo").value.trim();
  if (codigo === "") {
    mostrarStatus(this.querySelector("div[role=status]"), false, "Digite o código do gift card.");
    return;
  }
  enviarCarteira(this, "/api/giftcard", { codigo: codigo });
});

document.getElementById("form-cartao").addEventListener("submit", function (evento) {
  evento.preventDefault();
  const dados = {
    numero: document.getElementById("cartao-numero").value.trim(),
    cvc: document.getElementById("cartao-cvc").value.trim(),
    validade: document.getElementById("cartao-validade").value.trim(),
    limite: document.getElementById("cartao-limite").value.trim()
  };
  if (dados.numero === "" || dados.cvc === "" || dados.validade === "" || dados.limite === "") {
    mostrarStatus(this.querySelector("div[role=status]"), false, "Preencha todos os campos do cartão.");
    return;
  }
  enviarCarteira(this, "/api/cartoes", dados);
});


// ===== Administração =====

const estadoAdmin = { jogos: [], jogoEditado: null, tipoNovo: "gratuito", confirmandoRemocao: false, contaSelecionada: null, contas: [] };

// abas Jogos / Gift cards / Contas
document.querySelectorAll("button[data-aba-admin]").forEach(function (aba) {
  aba.addEventListener("click", function () {
    document.querySelectorAll("button[data-aba-admin]").forEach(function (outra) {
      outra.setAttribute("aria-selected", outra === aba ? "true" : "false");
    });
    document.getElementById("admin-jogos").hidden = aba.dataset.abaAdmin !== "jogos";
    document.getElementById("admin-gift").hidden = aba.dataset.abaAdmin !== "gift";
    document.getElementById("admin-contas").hidden = aba.dataset.abaAdmin !== "contas";
  });
});

function carregarAdmin() {
  carregarJogosAdmin();
  carregarGiftCards();
  carregarContasAdmin();
}

// --- Jogos ---

async function carregarJogosAdmin() {
  const resposta = await api("/api/jogos");
  if (!resposta.ok) {
    return;
  }
  estadoAdmin.jogos = resposta.jogos;

  const linhas = document.getElementById("admin-jogos-linhas");
  linhas.innerHTML = "";

  for (const jogo of resposta.jogos) {
    const editado = jogo.id === estadoAdmin.jogoEditado;
    const linha = criar("tr", editado ? "bg-selecionado shadow-[inset_3px_0_0_#2F6FE0]" : "");
    linha.appendChild(criar("td", "px-4 h-12 border-b border-linha font-mono text-suave", String(jogo.id)));

    const celula = criar("td", "px-2 border-b border-linha");
    const botao = criar("button", "min-h-11 w-full px-2 text-left font-medium hover:text-link", jogo.titulo);
    botao.type = "button";
    botao.addEventListener("click", function () { editarJogo(jogo); });
    celula.appendChild(botao);
    linha.appendChild(celula);

    linha.appendChild(criar("td", "px-4 border-b border-linha text-right font-mono", formatarTamanho(jogo.tamanho)));
    linha.appendChild(criar("td", "px-4 border-b border-linha text-right font-mono", formatarPreco(jogo)));
    linha.appendChild(criar("td", "px-4 border-b border-linha text-[#C9D1DB]", jogo.em_uso ? "Sim" : "Não"));
    linhas.appendChild(linha);
  }
}

// escolhe o tipo do jogo novo (o preço só vale para jogo pago)
function escolherTipoJogo(tipo) {
  estadoAdmin.tipoNovo = tipo;
  document.querySelectorAll("button[data-tipo-jogo]").forEach(function (botao) {
    botao.setAttribute("aria-pressed", botao.dataset.tipoJogo === tipo ? "true" : "false");
  });
  const preco = document.getElementById("jogo-preco");
  preco.disabled = tipo !== "pago";
  if (tipo !== "pago") {
    preco.value = "";
  }
  document.getElementById("jogo-preco-obrigatorio").hidden = tipo !== "pago";
}

document.querySelectorAll("button[data-tipo-jogo]").forEach(function (botao) {
  botao.addEventListener("click", function () { escolherTipoJogo(botao.dataset.tipoJogo); });
});

// deixa o formulário no modo "novo jogo"
function novoJogo() {
  estadoAdmin.jogoEditado = null;
  estadoAdmin.confirmandoRemocao = false;
  const formulario = document.getElementById("form-jogo");
  formulario.reset();
  document.getElementById("form-jogo-titulo").textContent = "Novo jogo";
  document.getElementById("form-jogo-salvar").textContent = "Cadastrar jogo";
  document.getElementById("form-jogo-novo").hidden = true;
  document.getElementById("jogo-senha-obrigatoria").hidden = false;
  document.getElementById("jogo-senha-dica").hidden = true;
  document.getElementById("jogo-tipo-dica").hidden = true;
  document.querySelectorAll("button[data-tipo-jogo]").forEach(function (b) { b.disabled = false; });
  escolherTipoJogo("gratuito");
  desenharRemocaoJogo();
  carregarJogosAdmin();
}

// preenche o formulário com o jogo clicado
function editarJogo(jogo) {
  estadoAdmin.jogoEditado = jogo.id;
  estadoAdmin.confirmandoRemocao = false;

  document.getElementById("form-jogo-titulo").textContent = "Editar jogo";
  document.getElementById("form-jogo-salvar").textContent = "Salvar alterações";
  document.getElementById("form-jogo-novo").hidden = false;
  document.getElementById("jogo-titulo").value = jogo.titulo;
  document.getElementById("jogo-usuario").value = jogo.usuario;
  document.getElementById("jogo-senha").value = "";
  document.getElementById("jogo-senha-obrigatoria").hidden = true;
  document.getElementById("jogo-senha-dica").hidden = false;
  document.getElementById("jogo-tamanho").value = String(jogo.tamanho).replace(".", ",");

  // o tipo fica travado: no C++ um jogo gratuito não vira pago
  escolherTipoJogo(jogo.pago ? "pago" : "gratuito");
  document.querySelectorAll("button[data-tipo-jogo]").forEach(function (b) { b.disabled = true; });
  document.getElementById("jogo-tipo-dica").hidden = false;
  if (jogo.pago) {
    document.getElementById("jogo-preco").value = String(jogo.preco).replace(".", ",");
  }

  document.querySelector("#form-jogo div[role=status]").hidden = true;
  desenharRemocaoJogo();
  carregarJogosAdmin();
}

document.getElementById("form-jogo-novo").addEventListener("click", novoJogo);

// botão Remover (só aparece editando), com confirmação
function desenharRemocaoJogo() {
  const area = document.getElementById("form-jogo-remocao");
  area.innerHTML = "";
  area.hidden = estadoAdmin.jogoEditado === null;
  if (area.hidden) {
    return;
  }

  if (!estadoAdmin.confirmandoRemocao) {
    const botao = criar("button", "min-h-11 rounded-md border border-[#4A3818] text-aviso font-medium hover:bg-[#2A2113]", "Remover jogo");
    botao.type = "button";
    botao.addEventListener("click", function () {
      estadoAdmin.confirmandoRemocao = true;
      desenharRemocaoJogo();
    });
    area.appendChild(botao);
    return;
  }

  area.appendChild(criar("p", "text-aviso", "Remover " + document.getElementById("jogo-titulo").value + " da loja?"));
  const botoes = criar("div", "grid grid-cols-2 gap-2");
  const cancelar = criar("button", "min-h-11 rounded-md border border-borda hover:border-suave", "Cancelar");
  cancelar.type = "button";
  cancelar.addEventListener("click", function () {
    estadoAdmin.confirmandoRemocao = false;
    desenharRemocaoJogo();
  });
  const confirmar = criar("button", "min-h-11 rounded-md bg-[#8A5A12] text-white font-semibold hover:brightness-110", "Remover");
  confirmar.type = "button";
  confirmar.addEventListener("click", async function () {
    const status = document.querySelector("#form-jogo div[role=status]");
    const resposta = await api("/api/admin/jogos/remover", { id: estadoAdmin.jogoEditado });
    const texto = ultimaMensagem(resposta, resposta.ok ? "Jogo removido." : "Não foi possível remover.");
    if (resposta.ok) {
      novoJogo();
      carregarCatalogo();
    } else {
      estadoAdmin.confirmandoRemocao = false;
      desenharRemocaoJogo();
    }
    mostrarStatus(status, resposta.ok, texto);
  });
  botoes.appendChild(cancelar);
  botoes.appendChild(confirmar);
  area.appendChild(botoes);
}

// cadastrar ou salvar alterações
document.getElementById("form-jogo").addEventListener("submit", async function (evento) {
  evento.preventDefault();
  const status = this.querySelector("div[role=status]");
  const editando = estadoAdmin.jogoEditado !== null;

  const dados = {
    titulo: document.getElementById("jogo-titulo").value.trim(),
    usuario: document.getElementById("jogo-usuario").value.trim(),
    senha: document.getElementById("jogo-senha").value,
    tamanho: document.getElementById("jogo-tamanho").value.trim(),
    preco: document.getElementById("jogo-preco").value.trim(),
    tipo: estadoAdmin.tipoNovo
  };

  if (dados.titulo === "" || dados.usuario === "" || dados.tamanho === "" ||
      (!editando && dados.senha === "") || (estadoAdmin.tipoNovo === "pago" && dados.preco === "")) {
    mostrarStatus(status, false, "Preencha todos os campos obrigatórios.");
    return;
  }

  let resposta;
  if (editando) {
    dados.id = estadoAdmin.jogoEditado;
    resposta = await api("/api/admin/jogos/editar", dados);
  } else {
    resposta = await api("/api/admin/jogos", dados);
  }

  const texto = ultimaMensagem(resposta, resposta.ok ? "Feito." : "Não foi possível salvar.");
  if (resposta.ok) {
    if (!editando) {
      novoJogo();
    } else {
      carregarJogosAdmin();
      document.getElementById("jogo-senha").value = "";
    }
    carregarCatalogo();
  }
  mostrarStatus(status, resposta.ok, texto);
});

// --- Gift cards ---

async function carregarGiftCards() {
  const resposta = await api("/api/admin/giftcards");
  if (!resposta.ok) {
    return;
  }

  const linhas = document.getElementById("admin-gift-linhas");
  linhas.innerHTML = "";
  for (const card of resposta.giftcards) {
    const linha = criar("tr");
    linha.appendChild(criar("td", "px-4 h-12 border-b border-linha font-mono", card.codigo));
    linha.appendChild(criar("td", "px-4 border-b border-linha text-right font-mono", formatoReais.format(card.valor)));
    const celula = criar("td", "px-4 border-b border-linha");
    celula.appendChild(criar("span", card.valido
      ? "inline-block px-2.5 py-0.5 rounded-full text-[13px] font-medium bg-[#13233B] text-[#A9CBFF] border border-[#23406A]"
      : "inline-block px-2.5 py-0.5 rounded-full text-[13px] font-medium bg-elevado text-[#B4BDC9] border border-[#2F3846]",
      card.valido ? "Disponível" : "Usado"));
    linha.appendChild(celula);
    linhas.appendChild(linha);
  }
  document.getElementById("admin-gift-vazio").hidden = resposta.giftcards.length > 0;
}

document.getElementById("form-admin-gift").addEventListener("submit", async function (evento) {
  evento.preventDefault();
  const status = this.querySelector("div[role=status]");
  const codigo = document.getElementById("admin-gift-codigo").value.trim();
  const valor = document.getElementById("admin-gift-valor").value.trim();

  if (codigo === "" || valor === "") {
    mostrarStatus(status, false, "Preencha o código e o valor.");
    return;
  }

  const resposta = await api("/api/admin/giftcards", { codigo: codigo, valor: valor });
  if (resposta.ok) {
    this.reset();
    carregarGiftCards();
  }
  mostrarStatus(status, resposta.ok, ultimaMensagem(resposta, resposta.ok ? "Gift card criado." : "Não foi possível criar."));
});

// --- Contas ---

async function carregarContasAdmin() {
  const resposta = await api("/api/admin/contas");
  if (!resposta.ok) {
    return;
  }
  estadoAdmin.contas = resposta.contas;

  // se a conta selecionada não existe mais, limpa a seleção
  if (!resposta.contas.some(function (c) { return c.id === estadoAdmin.contaSelecionada; })) {
    estadoAdmin.contaSelecionada = null;
  }

  const linhas = document.getElementById("admin-contas-linhas");
  linhas.innerHTML = "";
  for (const conta of resposta.contas) {
    const selecionada = conta.id === estadoAdmin.contaSelecionada;
    const linha = criar("tr", selecionada ? "bg-selecionado shadow-[inset_3px_0_0_#2F6FE0]" : "");
    linha.appendChild(criar("td", "px-4 h-12 border-b border-linha font-mono text-suave", String(conta.id)));

    const celula = criar("td", "px-2 border-b border-linha");
    const botao = criar("button", "min-h-11 w-full px-2 text-left font-medium hover:text-link", conta.nome);
    botao.type = "button";
    botao.addEventListener("click", function () {
      estadoAdmin.contaSelecionada = conta.id;
      document.querySelector("#form-remover-conta div[role=status]").hidden = true;
      carregarContasAdmin();
    });
    celula.appendChild(botao);
    linha.appendChild(celula);

    linha.appendChild(criar("td", "px-4 border-b border-linha text-right font-mono", formatoReais.format(conta.saldo)));
    linha.appendChild(criar("td", "px-4 border-b border-linha text-right font-mono", String(conta.jogos)));
    linhas.appendChild(linha);
  }
  document.getElementById("admin-contas-vazio").hidden = resposta.contas.length > 0;

  const escolhida = resposta.contas.find(function (c) { return c.id === estadoAdmin.contaSelecionada; });
  document.getElementById("remover-conta-vazio").hidden = !!escolhida;
  document.getElementById("remover-conta-campos").hidden = !escolhida;
  if (escolhida) {
    document.getElementById("remover-conta-nome").textContent = escolhida.nome;
  }
}

document.getElementById("form-remover-conta").addEventListener("submit", async function (evento) {
  evento.preventDefault();
  const status = this.querySelector("div[role=status]");
  const senha = document.getElementById("remover-conta-senha").value;

  if (senha === "") {
    mostrarStatus(status, false, "Digite a senha da conta.");
    return;
  }

  const resposta = await api("/api/admin/contas/remover", { id: estadoAdmin.contaSelecionada, senha: senha });
  document.getElementById("remover-conta-senha").value = "";

  if (resposta.ok) {
    // se a conta removida era a logada, a tela volta para o modo deslogado
    if (contaAtual !== null && resposta.conta === null) {
      atualizarSessao(null);
    }
    carregarContasAdmin();
    carregarJogosAdmin();
  }
  mostrarStatus(status, resposta.ok, ultimaMensagem(resposta, resposta.ok ? "Conta removida." : "Não foi possível remover."));
});


// ===== Início =====

api("/api/status");
carregarCatalogo();

// se alguém já estava logado (ex.: a página foi recarregada), continua logado
api("/api/sessao").then(function (resposta) {
  if (resposta.ok) {
    atualizarSessao(resposta.conta);
  }
});
