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

// chama um caminho da API; sem "dados" faz um GET, com "dados" faz um POST com JSON
// as mensagens que vierem na resposta já aparecem no painel
async function api(caminho, dados) {
  const opcoes = dados === undefined
    ? {}
    : { method: "POST", headers: { "Content-Type": "application/json" }, body: JSON.stringify(dados) };

  try {
    const resposta = await fetch(caminho, opcoes);
    const json = await resposta.json();

    if (json.mensagens) {
      mostrarMensagens(json.mensagens);
    }
    return json;
  } catch (erro) {
    mostrarMensagens(["Não foi possível falar com o servidor. Ele está rodando?"]);
    return { ok: false };
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

  // a compra entra no passo 5; por enquanto o botão fica desativado
  const botao = criar("button", "min-h-11 rounded-md bg-borda text-suave font-semibold cursor-not-allowed",
    jogo.pago ? "Comprar" : "Adicionar à biblioteca");
  botao.type = "button";
  botao.disabled = true;
  painel.appendChild(botao);
  painel.appendChild(criar("p", "text-[13px] text-suave", "Entre na conta para adicionar jogos à sua biblioteca."));
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


// ===== Início =====

api("/api/status");
carregarCatalogo();
